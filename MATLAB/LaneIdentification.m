% Read the video and start a video player
videoFReader = vision.VideoFileReader("roadtrip2.mov");		
videoPlayer = vision.VideoPlayer; 

% Start looping through each video frame until the video is done
frameNum = 0;
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
    figure ('Name','ROI Mask');  				% create a figure for ROI
    %imshow(imageBWROI);                         % display ROI image
    % Adds the mask to the canny filter
    imageBWMasked = immultiply(croppedVidFrame, imageBWROI);

    % Hough transform
    [H,T,R] = hough(imageBWMasked);                     % hough transform
    %figure('Name','Hough Image');                       % create a figure for masked image
    %imshow(H);                                          % show hough space image
    noLines = 8;                                        % define no. of lines
    peaks = houghpeaks(H,noLines,'threshold',ceil(0.3*max(H(:))))
    
    lines = houghlines(imageBWMasked,T,R,peaks,'FillGap',15,'MinLength', 30) 
    lengthLine = length(lines)
    xy = zeros(lengthLine, 4);
    a = imageSize(2)*0.25;
    b = imageSize(1)*.66;
    for k = 1:lengthLine
        xy(k,1:4) = [(lines(k).point1(1)+a) (lines(k).point1(2)+b) (lines(k).point2(1)+a) (lines(k).point2(2)+b)]
    end
    
    LaneID = insertShape(videoFrame,'line',xy,'LineWidth',2, 'Color','red');
    %figure('Name','Land Identification');			% create a figure for masked image
    imshow(LaneID); 
    pause(.2)
end