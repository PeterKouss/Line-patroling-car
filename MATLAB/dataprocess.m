
Lv=1./(Lraw.*390./6e6);
figure;
subplot(3,1,1)
plot(Lv);
%% filter
 fs=linspace(-6e6/min(Lraw)/2,6e6/min(Lraw)/2,length(Lv));
 subplot(3,1,2);
 plot(fs,abs(fftshift(fft(Lv))));

Hd=iir_filter_50_100;
Lv=filter(Hd,Lv);
subplot(3,1,3);
plot(Lv);
%% resample
Lv= [0; Lv(2:length(Lv))];
Ldt=Lraw./6e6;
Lt=cumsum(Ldt);
Lt = [0; Lt(2:length(Lt))];
Lts=timeseries(Lv,Lt);
Lts1=resample(Lts,[0:0.0001:1],'linear');
Lv1=Lts1.data;
figure;
plot(Lts);
hold on;
plot(Lts1,'bo');
%% Generate Input series for systemIdentification
scale=50;
u=ones(1,length(Lv1))*scale;
U=u';
D=Lv1;
data = iddata(D,U,0.0001);
sys = tfest(data,2,0)
figure;
step(scale*sys);
hold on;
plot(Lts1);
p=pole(sys);
tao1=-1/p(1);
tao2=-1/p(2);


%% Kp plot
k=126.1/2268;
a=[tao1*tao2 tao1+tao2 1];
Gs=tf(k,a);
% figure;
% for z=0.1:0.1:2
%     kp=(((a(2))/(2*z)).^2)/(k*(a(1)));
%     wn=sqrt((1+k*kp)/(a(1)));
%     num=wn^2;
%     den=[1 2*z*wn wn^2];
%     Hs=tf(num,den);
%     step(Hs)
%     hold on;
% end
    z_s=1.3;
    kp_s=(((a(2))/(2*z_s)).^2)/(k*(a(1))); %建模得到kp最佳值，根据20%，50%，80%占空比分别为44.2253,38.8691,13.3203
    disp('z  Kp')
    disp([z_s kp_s]);

%% simulation
Kp=kp_s;
u=50;
dr=0;
dt=[0 0.0095];
x=6;
for n=2:400
    u(n)=(1/k)*x+Kp*(x-dr(n-1));
    T=0:0.001:dt(n);
    U=interp1(dt(1:n),u,T,'nearest');
    for i=1:length(U)
        if U(i)>100
            U(i)=99;
        elseif U(i)<20
            U(i)=20;
        end
    end
    u(n)=U(length(U));
    Rs=lsim(Gs,U,T);
    dr(n)=Rs(length(Rs));
    dt(n+1)=dt(n)+1/(dr(n)*390);
end
figure;
subplot(2,1,1);
plot(dt(1:length(dt)-1),dr);
subplot(2,1,2);
plot(dt(1:length(dt)-1),u);


kout=1/k;


