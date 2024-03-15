% Read the video and start a video player
videoFReader = vision.VideoFileReader("roadtrip2.mov");		
videoPlayer = vision.VideoPlayer; 

% Start looping through each video frame until the video is done
frameNum = 0;
v = VideoWriter("export","Archival");
open(v)
while ~isDone(videoFReader)
    close all
    videoFrame = videoFReader(); 
    % Crop the frame
    imageSize = size(videoFrame);
    cropRect = [imageSize(2)*0.25 imageSize(1)*.66 imageSize(2)*0.5 imageSize(1)*0.35];
    croppedVidFrame = imcrop(videoFrame, cropRect);
    % Turn grayscale
    croppedVidFrame = rgb2gray(croppedVidFrame);
    % Gaussian blur
    croppedVidFrame = imgaussfilt(croppedVidFrame, 3);
    % Canny filter
    croppedVidFrame = edge(croppedVidFrame, 'canny'); 

    %Create ROI mask
    cropSize = size(croppedVidFrame);
    row = [cropSize(1) cropSize(1)*0.25 cropSize(1)*0.25 cropSize(1) cropSize(1)]; % ROI row vector (y-coordinates)
    col = [0 cropSize(2)*0.3 cropSize(2)*0.7 cropSize(2) 0]; 	% ROI col vector (x-coordinates)
    imageBWROI = roipoly(croppedVidFrame, col, row);	% create ROI polygon (trapezoid) mask

    % Adds the mask to the canny filter
    imageBWMasked = immultiply(croppedVidFrame, imageBWROI);

    % Hough transform
    [H,T,R] = hough(imageBWMasked);                     % hough transform
    noLines = 2;                                        % define no. of lines
    peaks = houghpeaks(H,noLines,'threshold',ceil(0.3*max(H(:))));
    
    lines = houghlines(imageBWMasked,T,R,peaks,'FillGap',15,'MinLength', 30); 
    lengthLine = length(lines);
    xy = zeros(lengthLine, 4);
    a = imageSize(2)*0.25; % Corrects for the cropping, allowing the line
    b = imageSize(1)*.66; % to be drawn on the correct part of the image

    for k = 1:lengthLine
        % If the slope of the line is too high
        disp(lines(k))
        run = lines(k).point2(1) - lines(k).point1(1);
        rise = lines(k).point2(2) - lines(k).point1(2);
        % If the line is a horizontal line, don't count it
        if (abs(rise) / abs(run) < .2)
            xy(k,1:4) = [NaN NaN NaN NaN]
            continue
        end
        xy(k,1:4) = [(lines(k).point1(1)+a) (lines(k).point1(2)+b) (lines(k).point2(1)+a) (lines(k).point2(2)+b)];
    end
    xy = rmmissing(xy)
    polygon = [];
    for k = 1:lengthLine
        polygon = [polygon, xy(k, :)]
    end

    LaneID = insertShape(videoFrame,'line',xy,'LineWidth',2, 'Color','red');
    LaneID = insertShape(LaneID, 'filled-polygon', polygon, ShapeColor=["white"],Opacity=0.7);
    %imshow(LaneID); 
    writeVideo(v,LaneID)
    %pause(.2)
end
close(v)