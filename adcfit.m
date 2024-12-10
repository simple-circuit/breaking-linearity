clear
a=csvread('adcdata.csv');
m=length(a);
n=linspace(1,m,m);
x=a';
f=polyfit(n,x,1)
# f =f + [ 0.0005e-4, -0.0001 ] #use to adjust polyfit
y=polyval(f,n);
e=x-y;
p=polyfit(x,e,3)
yerr=polyval(p,x);
plot(x,e,x,yerr)
axis([-2,2,-0.003,0.003]);
grid on