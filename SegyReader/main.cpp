#include <iostream>
#include "INSMSegyReader.h"
#define PI 3.1415926535

//int main() {
//	printf("float size %d\n",sizeof(float));
//	printf("char size %d\n", sizeof(char));
//	printf("short size %d\n", sizeof(short));
//	printf("long size %d\n", sizeof(long));
//	printf("unsigned short size %d\n", sizeof(unsigned short));
//	printf("int size %d\n", sizeof(int));
//	/*union SI si;
//	strcpy(si.s,"a");
//	printf("%d,%s", si.a, si.s);
//	return 0;*/
//}

//最小二乘
float deal(float x[5], float y[5])//采用克莱默法则求解方程
{
	int select = 2;
	int i;
	float k;
	float a0, a1, a2, temp, temp0, temp1, temp2;
	float sy = 0, sx = 0, sxx = 0, syy = 0, sxy = 0, sxxy = 0, sxxx = 0, sxxxx = 0;//定义相关变量
	for (i = 0; i<5; i++)
	{
		sx += x[i];//计算xi的和
		sy += y[i];//计算yi的和
		sxx += x[i] * x[i];//计算xi的平方的和
		sxxx += powf(x[i], 3);//计算xi的立方的和
		sxxxx += powf(x[i], 4);//计算xi的4次方的和
		sxy += x[i] * y[i];//计算xi乘yi的的和
		sxxy += x[i] * x[i] * y[i];//计算xi平方乘yi的和
	}
	temp = 5 * sxx - sx*sx;//方程的系数行列式
	temp0 = sy*sxx - sx*sxy;
	temp1 = 5 * sxy - sy*sx;
	a0 = temp0 / temp;
	a1 = temp1 / temp;
	temp = 5 * (sxx*sxxxx - sxxx*sxxx) - sx*(sx*sxxxx - sxx*sxxx)//方程的系数行列式
		+ sxx*(sx*sxxx - sxx*sxx);
	temp0 = sy*(sxx*sxxxx - sxxx*sxxx) - sxy*(sx*sxxxx - sxx*sxxx)
		+ sxxy*(sx*sxxx - sxx*sxx);
	temp1 = 5 * (sxy*sxxxx - sxxy*sxxx) - sx*(sy*sxxxx - sxx*sxxy)
		+ sxx*(sy*sxxx - sxy*sxx);
	temp2 = 5 * (sxx*sxxy - sxy*sxxx) - sx*(sx*sxxy - sy*sxxx)
		+ sxx*(sx*sxy - sy*sxx);
	a0 = temp0 / temp;
	a1 = temp1 / temp;
	a2 = temp2 / temp;
	if (select == 2)
	{
		//printf("经最小二乘法拟合得到的二次近似方程为:\n");
		//printf("f(x)=%3.3fx2+%3.3fx+%3.3f\n", a2, a1, a0);
		k = 2 * a2*x[3] + a1;
		//printf("该点的斜率为%3.3f\n", k);
	}
	return k;
}

int main() {
	CINSMSegyReader st("C:\\Users\\skx\\Desktop\\new\\zdm_imp.sgy", 6);

	st.ReadSegyFile();
	std::cout << st.getTraceCount() << "," << st.getiLineCount() << "," << st.getxLineCount() << "," << st.getiLineStart() << "," << st.getxLineStart();
	auto data = st.getData()[0];
	std::cout << std::endl;
	for (auto v : data) {
		std::cout << v.a << " ";
	}
	std::cout << std::endl;
	system("pause");
	return 0;
}
/*
int main_2()
{
   std::vector<float> A;
   std::vector<float> A_; //地震原始数据
   std::vector<float> Tn_len;
   std::vector<float> BW;
   std::vector<float> AZCF;
   long Tn;
   int i,j;
   int k;
   char ch;
   float dat;
   union SI si;//采样点数
   union SP sp;//定义采样率
   union Data data;//定义数据
   union Line line;//线号
   union Trace trace;//道号
   union X_cor x_cor;//X坐标
   union Y_cor y_cor;//Y坐标
 
   FILE* fp, *fp1,*fp2;
   fp = fopen_s("C:\\Users\\skx\\Desktop\\new\\zdm_imp.sgy","rb");//打开SEGY数据文件
   //fp = fopen_s("E:\\test.sgy","rb");//打开SEGY数据文件
   if(!fp)
   {
       printf("can't open file!");
       exit(1);
   }

   fseek(fp,3220L,0);
   fread(si.s,2,1,fp);//读取采样点，并将其从big endian 转换到 little endian
   ch=si.s[0];
   si.s[0]=si.s[1];
   si.s[1]=ch;
   fseek(fp,3216L,0);//读取采样率，并将其从big endian 转换到 little endian
   fread(sp.s,2,1,fp);
   ch=sp.s[0];
   sp.s[0]=sp.s[1];
   sp.s[1]=ch;
   j=0;
   rewind(fp);
   while(!feof(fp))//检测文件大小,检测后必须减1
   {
      fread(&k,1,1,fp);
	  j+=1;
   }
   j=j-1;
   
   Tn = (short)((j-3600)/(float)(si.a*4+240));//计算道数 
   if(!(fp1=fopen_s("C:\\Users\\skx\\Desktop\\new\\test.txt","w+")))//新建文本文件存储数据
   { 
	   printf("can't open text file!");
       exit(1);  
   }
   if (!(fp2 = fopen_s("C:\\Users\\skx\\Desktop\\new\\test1.txt", "w+")))//新建文本文件存储数据
   {
	   printf("can't open text file!");
	   exit(1);
   }
   fprintf(fp1,"si = %u, sp =%u, Tn =%d, j=%d\n",si.a, sp.i, Tn, j);

   for(j=0;j<Tn;j++)
   {
//	fprintf(fp1,"÷÷÷÷÷÷÷÷÷÷÷÷÷÷The %d trace:－－－－－－－－－－－－－－\n ", j+1);
    //printf("The %f percent trace  \n ",100*(float)(j+1)/(float)Tn);
	for(i=0;i<si.a;i++)
    {
      
      fseek(fp,3600L+j*240L+j*si.a*4L+8L,0);//读取线号，并将其从big endian 转换到 little endian
	  fread(line.s,4,1,fp);
	  ch=line.s[0];
	  line.s[0]=line.s[3];
	  line.s[3]=ch;
	  ch=line.s[1];
	  line.s[1]=line.s[2];
	  line.s[2]=ch;

	  fseek(fp,3600L+j*240L+j*si.a*4L+20L,0);//读取道号，并将其从big endian 转换到 little endian
	  fread(trace.s,4,1,fp);
	  ch=trace.s[0];
	  trace.s[0]=trace.s[3];
	  trace.s[3]=ch;
	  ch=trace.s[1];
	  trace.s[1]=trace.s[2];
	  trace.s[2]=ch;

	  fseek(fp,3600L+j*240L+j*si.a*4L+72L,0);//读取X坐标，并将其从big endian 转换到 little endian
	  fread(x_cor.s,4,1,fp);
	  ch=x_cor.s[0];
	  x_cor.s[0]=x_cor.s[3];
	  x_cor.s[3]=ch;
	  ch=x_cor.s[1];
	  x_cor.s[1]=x_cor.s[2];
	  x_cor.s[2]=ch;

	  fseek(fp,3600L+j*240L+j*si.a*4L+76L,0);//读取Y坐标，并将其从big endian 转换到 little endian
	  fread(y_cor.s,4,1,fp);
	  ch=y_cor.s[0];
	  y_cor.s[0]=y_cor.s[3];
	  y_cor.s[3]=ch;
	  ch=y_cor.s[1];
	  y_cor.s[1]=y_cor.s[2];
	  y_cor.s[2]=ch;

	  fseek(fp,3600L+(j+1)*240L+j*si.a*4L+i*4L,0);
	  fread(data.dat,4,1,fp);//读取地震数据，并将其从big endian 转换到 little endian
	  ch=data.dat[0];
	  data.dat[0]=data.dat[3];
	  data.dat[3]=ch;
	  ch=data.dat[1];
	  data.dat[1]=data.dat[2];
	  data.dat[2]=ch;

	  

	  //计算反射强度
	  float h = data.a / (PI*(i + 1));
	  A.push_back(sqrt(data.a*data.a + h*h));
	  A_.push_back(data.a);
	 // fprintf(fp1,"	%d     %d     %d     %u      %u     %d \n ",j+1,line.a,trace.a,x_cor.a,y_cor.a,i*sp.i/2000+1);
	  //fprintf(fp1,"	 %.3f\n ",data.a);
	  fprintf(fp1,"	%d     %d     %d     %u      %u     %d     %.3f\n ",j+1,line.a,trace.a,x_cor.a,y_cor.a,i+1,data.a);
//	  fprintf(fp1," %d    %d      %f\n ",line.a,-i*sp.i/2000+1,data.a);

	  //输出顺序：总的道号、线号、道号、X坐标、Y坐标、采样点号、地震数据
	  if(fabs(data.a)>1e6)
		  printf("error");
    }
   }
   //--------振幅--------
   //--------计算弧长-------
   for (j = 0; j < Tn; j++)
   {
	   float T_len = 0;
	   float temp = si.a*sp.i;
	   for (i = 0; i < si.a; i++) {
		   if (((j*si.a) + i + 1) >= (((Tn-1)*si.a) + si.a)) break;
		   T_len += sqrt(powf((A[(j*si.a)+i+1]-A[(j*si.a)+i]),2) + pow(sp.i,2));
	   }
	   T_len /= temp;
	   Tn_len.push_back(T_len);
   }
   //-----计算带宽-----
   bool isH = false,isT = false;
   for (i = 0; i < Tn*si.a; i++) {
	   if (i < 2) {
		   i += 2;
		   isH = true;
	   }
	   else if (i > Tn*si.a - 3) {
		   i -= 2;
		   isT = true;
	   }
	   //采用当前值作为中间数，用前后共5个数据求得导数，最前、最后各两个数斜率按照最近的计算
	   float aa[5] = {A[i-2],A[i - 1] ,A[i] ,A[i + 1] ,A[i + 2] };
	   float bb[5] = {i-2,i-1,i,i+1,i+2};
	   
	   if (isH) {
		   i -= 2;
		   isH = 0;
	   }
	   else if (isT) {
		   i += 2;
		   isT = 0;
	   }

	   BW.push_back((deal(bb,aa)) / (2 * PI*A[i]));
   }
	   

	  
   //-----过零值平均频数----使用原始地震数据计算
   for (j = 0; j < Tn; j++) {
	   int n=0, t1=-1, t2=-1;
	   int oldtag=0, newtag=0; //前后两个值的状态记录 -1 负;1 正 ;0 零

	   if (A_[j*si.a] < 0) oldtag = newtag = -1;
	   else if(A_[j*si.a] > 0) oldtag = newtag = 1;
	   else {
		   oldtag = newtag = 0; t1 = 0; n++;
	   }

	   for (i = 1; i < si.a; i++) {
			if (A_[(j*si.a) + i] < 0) newtag = -1;
			else if (A_[(j*si.a) + i] > 0) newtag = 1;
			else {
				newtag = 0;
				t2 = i;// *sp.i;
				n++;
				if (t1 == -1)t1 = i;// *sp.i;
				oldtag = newtag;
				continue;
			}
			if (newtag != oldtag && oldtag != 0 ) {
				if (t1 == -1)t1 = i;// *sp.i;
				t2 = i;// *sp.i;
				n++;
			}
			oldtag = newtag;
	   }
	   AZCF.push_back((n-1)/(2*(t2-t1)));
   }
   fprintf(fp2,"道号     弧长     过零点     带宽\n");
   for (j = 0; j < Tn; j++) {
	   for (i = 0; i < si.a; i++) {
		   fprintf(fp2, "%-8d %-8.3f %-8.3f %-8.5f\n", j + 1, Tn_len[j], AZCF[j], BW[(j*si.a) + i]);
	   }

   }
   fclose(fp1);
   fclose(fp);
   return 0;

}
*/