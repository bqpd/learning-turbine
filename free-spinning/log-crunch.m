% turbine spun at ~5.3 Hz
log = load('accel4-27-5pm.log');
st = log(:,1);
x = log(:,2);
y = log(:,3);
z = log(:,4);
%%
size = 2048;
start = 6000;
nyq = floor(size/2);
%%

data = z(start:start+size-1)*(.99e-3)*(9.81); % in m/s^2;
t = st(start:start+size-1)/800; % in s
data = data - mean(data); % remove DC component

%%

subplot(2,2,1);
plot(t,data),title('Chosen data');
xlabel('time (sec)'), ylabel('acceleration (m/s^2)');

window = hamming(size);
wdata = data.*window;
subplot(2,2,2);
plot(t,wdata),title('Windowed data');
xlabel('time (sec)'), ylabel('acceleration (m/s^2)');

f_wd = abs(fft(wdata)).^2;
f_wdn = f_wd(1:nyq)/max(f_wd);
F = [0:nyq-1]./nyq*400; % max frequency just under 100 hz
subplot(2,2,3);
plot(F,f_wdn),title('FFT (normalized plot)');
xlabel('frequency (hertz)');

subplot(2,2,4);
semilogy(F,f_wdn),title('FFT (normalized log plot)');
xlabel('frequency (hertz)');

%%
peaks = zeros(size-2,1);
for i=2:size-1
    if data(i) >= data(i-1)
        if data(i) > data(i+1)
            peaks(i)=1;
        else
            peaks(i)=0;
        end
    else
        peaks(i)=0;
    end
end
peaks = peaks.*data(2:size);
% figure
% plot(peaks)