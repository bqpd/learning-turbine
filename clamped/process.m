clear all
%%
smpl_sec = 200;
full = load('data4.txt')*(.99e-3)*(9.81); % in m/s^2
size = 512;
nyq = floor(size/2);
start = 2700;
data = full(start+1:start+size);
t = [0:size-1]./smpl_sec; % around smpl_sec samples/second
data = data - mean(data); % remove DC component

subplot(2,2,1);
plot(t,data),title('Chosen data (DC component removed)'),axis([0 max(t) -50 50]);
xlabel('time (sec)'), ylabel('acceleration (m/s^2)');

window = hamming(512,'periodic');
wdata = data.*window;
%subplot(2,2,2);
%plot(t,wdata),title('Windowed data'),axis([0 max(t) -100 100]);
%xlabel('time (sec)'), ylabel('acceleration (m/s^2)');

f_wd = abs(fft(wdata)).^2;
f_wdn = f_wd(1:nyq)/max(f_wd);
F = [0:nyq-1]./nyq*smpl_sec/2; % max frequency just under 100 hz
%subplot(2,2,2);
%plot(F,f_wdn),title('FFT (normalized plot)');
%xlabel('frequency (hertz)');

subplot(2,2,2);
semilogy(F,f_wdn),title('FFT (normalized log plot)');
xlabel('frequency (hertz)');

%%
peaks = zeros(size-2,1);
for i=1:size-2
    p = i+1;
    result = 0;
    if data(p) >= data(p+1)
     %   if data(p) > data(p+2)
            if data(p) >= data(p-1)
    %            if data(p) > data(p-2)
                if data(p) > 0
                    result = 1;
                end
            end
    end
    peaks(i) = result;
end
peaks = peaks.*data(2:size-1);
[r, c, peaks] = find(peaks);
x = 0:length(peaks)-1;
ldec = log(peaks(1)./peaks);
slope = (x*ldec)/(x*x');
zeta = slope/sqrt(slope^2+4*pi^2)
subplot(2,2,3);
plot(x,ldec,'b.',x,slope.*x,'r-'),title('Log decrement of peak height (w/linear damping fit)');
xlabel('peak number');
%% model
u_slope = sqrt((sum((ldec'-slope.*x).^2)/(length(peaks)-2))/sum((x-mean(x)).^2))*2;
u_zeta = u_slope*(1/sqrt(slope^2+4*pi^2)+-slope^2*(slope^2+4*pi^2)^-1.5)
[a, i] = max(f_wdn);
wd = F(i)
wn = wd/sqrt(1-zeta^2);