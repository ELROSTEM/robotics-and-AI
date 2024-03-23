% Read the video and start a video player
videoFReader = vision.VideoFileReader("roadtrip2.mov");		
videoPlayer = vision.VideoPlayer; 

% Start looping through each video frame until the video is done
frameNum = 0;
v = VideoWriter("export","MPEG-4");
open(v)
while ~isDone(videoFReader)
    videoFrame = videoFReader();
    % Initial crop
    imageSize = size(videoFrame);
    cropRect = [imageSize(2)*0.2 imageSize(1)*.66 imageSize(2)*0.6 imageSize(1)*0.35];
    croppedVidFrame = imcrop(videoFrame, cropRect);

    % Initial blur and edge detection 
    filteredVidFrame = initialFilter(croppedVidFrame);

    % Make ROI mask
    mask = makeMask(filteredVidFrame);
    
    % Adds the mask to the canny filter
    imageBWMasked = immultiply(filteredVidFrame, mask);

    % Hough transform
    xy = houghTransform(imageBWMasked, 16); % 2nd arg is noLines
    xy = rmmissing(xy);
    xy = convertToOriginal(xy, cropRect);

    % Create the polygon
    polygon = findCorners(xy, imageSize(1), imageSize(2));

    % Draw the image
    LaneID = insertShape(videoFrame,'line',xy,'LineWidth',2, 'Color','red');
    LaneID = insertShape(LaneID, 'filled-polygon', polygon, ShapeColor=["white"],Opacity=0.7);

    videoPlayer(LaneID)
    writeVideo(v,LaneID)
end
close(v)

function outputImg = initialFilter(inputImg)
    % Turn grayscale
    croppedVidFrame = rgb2gray(inputImg);
    % Gaussian blur
    croppedVidFrame = imgaussfilt(croppedVidFrame, 6);
    % Canny filter
    outputImg = edge(croppedVidFrame, 'canny'); 
end

function outputImg = makeMask(inputImg)
    %Create ROI mask
    cropSize = size(inputImg);
    % Row and column go with each other and correspond to a coordinate
    row = [cropSize(1) cropSize(1)*.7 cropSize(1)*0.2 cropSize(1)*0.2 cropSize(1)*.9 cropSize(1) cropSize(1)];
    col = [0           0              cropSize(2)*.5  cropSize(2)*.5  cropSize(2)    cropSize(2) 0];
    outputImg = roipoly(inputImg, col, row);    
end

function xy = houghTransform(inputImg, noLines)
    [H,T,R] = hough(inputImg);
    peaks = houghpeaks(H,noLines,'threshold',ceil(0.3*max(H(:))));
    
    lines = houghlines(inputImg,T,R,peaks,'FillGap',15,'MinLength', 10); 
    lengthLine = length(lines);
    xy = zeros(lengthLine, 4);

    % Makes sure there are no horizontal lines
    for k = 1:lengthLine
        run = lines(k).point2(1) - lines(k).point1(1);
        rise = lines(k).point2(2) - lines(k).point1(2);
        % If the line is a horizontal line, turn into NaN
        if (abs(rise) / abs(run) < .1)
            xy(k,1:4) = [NaN NaN NaN NaN];
            continue
        end
        % Else, just add it regularly
        xy(k,1:4) = [(lines(k).point1(1)) (lines(k).point1(2)) (lines(k).point2(1)) (lines(k).point2(2))];
    end
end

% The original sweeping algorithm for polygon
function [x, y] = sortPolygonVertices(x, y)
    % Find the unweighted mean of the vertices  :  
    cx = mean(x);
    cy = mean(y);
    
    % Find the angles:
    a = atan2(y - cy, x - cx);
    
    % Find the correct sorted order:
    [~, order] = sort(a);
    
    % Sort the x and y coordinates
    x = x(order);
    y = y(order);
end

% Other algorithm to look for corners of the lines
function polygon = findCorners(xy, imgHeight, imgWidth)
    polygon = [NaN, NaN, NaN, NaN, NaN, NaN, NaN, NaN]; % BL, TL, TR, BR
    % Converts the line matrix to a matrix of coordinates
    xySize = size(xy);
    newX = [];
    newY = [];
    for k = 1:xySize(1)
        newX = [newX; xy(k, 1)];
        newX = [newX; xy(k, 3)];
        newY = [newY; xy(k, 2)];
        newY = [newY; xy(k, 4)];
    end
    xy = [newX, newY];

    xySize = size(xy);
    for k = 1:xySize(1)
        coord = [xy(k, 1), xy(k, 2)]
        if coord(2) < (imgWidth / 2) % If left side of image
            disp("Left side")
            if isnan(polygon(1)) || (coord(1) > polygon(1)) % If bottom
                disp("Bottom")
                polygon(1) = xy(k, 1);
                polygon(2) = xy(k, 2);
                disp(polygon)
            end
            if isnan(polygon(3)) || (coord(1) < polygon(3)) % If top
                disp("Top")
                polygon(3) = xy(k, 1);
                polygon(4) = xy(k, 2);
                disp(polygon)
            end
        else % Else, if right side of image
            disp("Right side")
            if isnan(polygon(7)) || (coord(1) > polygon(7)) % If bottom
                disp("Bottom")
                polygon(7) = xy(k, 1);
                polygon(8) = xy(k, 2);
                disp(polygon)
            end
            if isnan(polygon(5)) || (coord(1) < polygon(5)) % If top
                disp("Top")
                polygon(5) = xy(k, 1);
                polygon(6) = xy(k, 2);
                disp(polygon)
            end
        end
    end
    polygon = rmmissing(polygon);
end

function xy = convertToOriginal(xy, cropRect)
    xySize = size(xy); 
    for k = 1:xySize(1)
        xy(k, 1) = xy(k, 1) + cropRect(1);
        xy(k, 2) = xy(k, 2) + cropRect(2);
        xy(k, 3) = xy(k, 3) + cropRect(1);
        xy(k, 4) = xy(k, 4) + cropRect(2);
    end
end