
function y = averaging_filter(x) 
persistent buffer;
if isempty(buffer)
    buffer = zeros(10,1);
end
y = zeros(size(x), class(x));
for i = 1:numel(x)
    buffer(2:end) = buffer(1:end-1);
    buffer(1) = x(i);
    y(i) = sum(buffer)/numel(buffer);
end
end

