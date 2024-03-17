% Read the video and start a video player
videoFReader = vision.VideoFileReader("roadtrip2.mov");		
videoPlayer = vision.VideoPlayer; 

% Start looping through each video frame until the video is done
frameNum = 0;
%v = VideoWriter("export","Archival");
%open(v)
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

    % Create the polygon
    vertices_x = [];
    vertices_y = [];
    xySize = size(xy); 
    polygon = [];
    for k = 1:xySize(1)
        vertices_x = [vertices_x, xy(k,1), xy(k,3)];
        vertices_y = [vertices_y, xy(k,2), xy(k,4)];
    end

    [newX, newY] = sortPolygonVertices(vertices_x, vertices_y);
    polygon = reshape([newX; newY], 1, []);

    % Draw the image
    LaneID = insertShape(croppedVidFrame,'line',xy,'LineWidth',2, 'Color','red');
    LaneID = insertShape(LaneID, 'filled-polygon', polygon, ShapeColor=["white"],Opacity=0.7);

    videoPlayer(LaneID)
    %writeVideo(v,LaneID)
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
    row = [cropSize(1) cropSize(1)*.7 cropSize(1)*0.25 cropSize(1)*0.25 cropSize(1)*.9 cropSize(1) cropSize(1)];
    col = [0           0              cropSize(2)*.5    cropSize(2)*.5      cropSize(2)    cropSize(2) 0];
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

function output = findPoints(inputImg, xy)
    % Given a matrix of lines, find points for a polygon, being the top and
    % bottom points that are closest to the center
    output = [0, 0, 0, 0]
    xySize = size(xy); 
    imgSize = size(inputImg);
    center = imgSize(1) / 2;
    for k = 1:xySize(1) % Loop through all lines
        % Check if the min x is to the left or right of center
        if min(xy(k,1), xy(k,3)) < center % To the left of center,
            % if the x value 
        else
            continue
        end
    end
end

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
