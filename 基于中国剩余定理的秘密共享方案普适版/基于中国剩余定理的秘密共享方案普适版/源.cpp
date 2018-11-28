#include<iostream>
#include<string.h>
#include<time.h>
extern "C"
{
#include"miracl.h"
#include"mirdef.h"
}
 

#define t_Max 100
#define n_Max 100


using namespace std;


void BigArrayTwotoOne(big ki[], big di[], big kd[], int t, int keynumber[]) {
	for (int i = 0; i < t; i++) //两个大数数组合并为一个，ki在前，di在后适配解方程函数
	{
		kd[i] = mirvar(0);
		kd[i + t] = mirvar(0);
		int tmp = keynumber[i];
		copy(ki[tmp], kd[i]);
		copy(di[tmp], kd[i + t]);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////
void BigArrayAdds(big a[], big sum ,int n) { //大数数组连加
	add(a[0], a[1], sum);										
	for (int i = 0; i < n - 2; i++)
	{
		add(sum, a[i + 2], sum);								
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////
void BigArrayMutlipies(big a[], big M/*result*/, int n) {  //大数数组连乘
	multiply(a[0], a[1], M);
	for (int r = 0; r < n - 2; r++) {
		multiply(M, a[r+2], M);								
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////
int congruent_equation(big am[], big result, int t) { //中国剩余定理解同余方程组
	big con = mirvar(1);
	big z = mirvar(0);
	int Size = t;
	int DoubleSize = 2 * t;
	for (int i = 0; i < Size; i++) {
		int tmp = (i + Size + 1) % (DoubleSize);
		if (tmp == 0) tmp = Size;
		egcd(am[i + Size], am[tmp], z);
		if (mr_compare(z, con) != 0) {
			cout << "不能直接利用中国剩余定理" << endl;
			system("pause");
			return -1;
		}
	}
	big M = mirvar(0);												//m
	big monij[t_Max];												//Mj^-1
	big Mj[t_Max];													//Mj=m/mi
	big xj[t_Max];													//xj=Mj*Mj^-1*ai mod m
	big Mtmp = mirvar(0);
	BigArrayMutlipies(&am[Size], M, Size);
	for (int i = 0; i < Size; i++) {
		Mj[i] = mirvar(0);
		monij[i] = mirvar(0);
		xj[i] = mirvar(0);
		copy(M, Mtmp);
		divide(Mtmp, am[Size + i], Mj[i]);							//Mj=m/mj
		xgcd(Mj[i], am[i + Size], monij[i], monij[i], monij[i]);		//monij=Mj^-1=Mj^-1 mod mi		
		multiply(Mj[i], am[i], xj[i]);
		multiply(xj[i], monij[i], xj[i]);							//xj[i] = mj*mj^-1*aj		               
		divide(xj[i], M, M);										//xj=xj mod M
	}
	BigArrayAdds(xj, result, Size);
	BOOL sign = divisible(result,M);
	divide(result, M, M);											//result取模
	return 0;
}
///////////////////////////////////////////////////////////////////////////////////////////


int encode(big ki[], big di[], int t, int n, big k/*已知明文*/, big N, big M) { //加密函数
	big tmp = mirvar(0);
	big con = mirvar(1);
	char uselessstring[9999] = { '\0' };
	int klen = cotstr(k, uselessstring);
	int klen_devide_n = klen / t;
	if (klen%t != 0) klen_devide_n += 1;
	irand((unsigned)time(0));
	for (int i = 0; i < n; i++) {     //选定不同位数的素数来满足条件
LOOP1:
		bigdig(klen_devide_n+i, 10, di[i]);
			if (isprime(di[i]) == FALSE) goto LOOP1;
			
	}
	BigArrayMutlipies(di, N, t);
	BigArrayMutlipies(&di[n - t + 1], M, t - 1);
	for (int i = 0; i < n; i++)
	{
		copy(k, tmp);
		divide(tmp, di[i], di[i]);
		copy(tmp, ki[i]);
	}
	return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////
int decode(big ki[], big di[], big k_, int keymuch) {    //解密
	int keynumber[n_Max];
	for (int i = 0; i < keymuch; i++)
	{
		keynumber[i] = i;
	}
	big kianddiinuse[2 * t_Max];
	BigArrayTwotoOne(ki, di, kianddiinuse, keymuch, keynumber);
	if (congruent_equation(kianddiinuse, k_, keymuch) == 0)
		return 0;
	else return -1;
}

//////////////////////////////////////////////////////////////////////////////////////////
int decodeauto(big k_, big di[], big ki[], int t, int n) {   //全自动模式下的解密
	int keynumber[t_Max];
	for (int i = 0; i < t; i++) {  //选定拿t组密钥用于解秘
LOOP2:
		srand((unsigned)time(NULL));
		keynumber[i] = rand() % n;
		for (int r = 0; r < i; r++) {
			if (keynumber[i] == keynumber[r]) goto LOOP2;
		}
	}

	big kianddiinuse[2*t_Max];
	BigArrayTwotoOne(ki, di, kianddiinuse, t, keynumber);
	if (congruent_equation(kianddiinuse, k_, t) == 0)
		return 0;
	else return -1;
}

////////////////////////////////////////////////////////////////////////////////////////////////
int everyauto(int t, int n) {  //全自动模式，以完成作业并做用户演示用，不生成文件
	miracl *mip = mirsys(10000, 10);
	big N = mirvar(0);
	big M = mirvar(0);
	big k = mirvar(0);//明文k
	irand((unsigned)time(0));
	bigdig(300, 10, k);
	cout << "明文k如下" << endl;
	cotnum(k, stdout);
	big di[n_Max];
	big ki[n_Max];
	for (int i = 0; i < n_Max; i++)
	{
		ki[i] = mirvar(0);
		di[i] = mirvar(0);
	}
	encode(ki, di, t, n, k, N, M);
	for (int i = 0; i < n; i++) {
		cout << "d" << i + 1 << "=";
		cotnum(di[i], stdout);
		cout << endl;
		cout << "k" << i + 1 << "=";
		cotnum(ki[i], stdout);
		cout << endl;
	}
	cout << "N=";
	cotnum(N, stdout);
	cout << "M=";
	cout << endl;
	cotnum(M, stdout);
	cout << endl;
	big k_ = mirvar(0); //密文
	if (decodeauto(k_, di, ki, t, n) == -1) {
		cout << "解密过程失败！" << endl;
		return -1;
	}
	cout << "解密后的结果为:";
	cotnum(k_, stdout);
	cout << endl;
	if (mr_compare(k, k_) == 0) {
		cout << "经mr_compare()函数检验，结果与明文相同。" << endl;
		mirexit();
		return 0;
	}
	else {
		cout << "经mr_compare()函数检验，结果与明文不同，程序运行失败。" << endl;
		mirexit();
		return -1;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////
int fileencode(int t, int n) {  //对存有一个大数的文件进行加密并输入n个密文文件
	miracl *mip = mirsys(10000, 10);
	char plaintextfile[200];
	cout << "输入明文文件（.txt格式）完整路径:";
	cin >> plaintextfile;
	cout << endl;
	char houzhui[] = ".txt";
	char *locate = strstr(plaintextfile, houzhui);
	if (locate == NULL) {
		cout << "文件格式错误!" << endl;
		mirexit();
		return -1;
	}
	FILE *fp;
	fp = fopen(plaintextfile, "r");
	if(fp==NULL) {
		cout << "文件输入发生错误！" << endl;
		mirexit();
		return -1;
	};
	big plaintextnum = mirvar(0);
	cinnum(plaintextnum, fp);
	fclose(fp);
	////////////////////明文文件处理完毕
	 //预备输出到原始文件路径
	int houzhuilocate = locate - plaintextfile; 
	char cipherfileend[] = "_ciphertext01.txt";
	char ciphertextfile[200] = { '\0' };
	strncpy(ciphertextfile, plaintextfile, houzhuilocate);
	strcat(ciphertextfile, cipherfileend);
	cout << "加密中，请稍等..." << endl;
	//输出的第一个文件名确定，开始做计算
	big N = mirvar(0);
	big M = mirvar(0);
	big ki[n_Max];
	big di[n_Max];
	for (int i = 0; i < n_Max; i++)
	{
		ki[i] = mirvar(0);
		di[i] = mirvar(0);
	}
	encode(ki, di, t, n, plaintextnum, N, M);
	FILE *password[n_Max] = { NULL };

	for (int i = 0; i < n; i++)
	{
		ciphertextfile[houzhuilocate+11] = (i + 1) / 10 + '0';
		ciphertextfile[houzhuilocate+12] = i % 10 + 1 + '0';
		password[i] = fopen(ciphertextfile, "w+");
		cotnum(ki[i], stdout);
		cotnum(di[i], stdout);
		cotnum(ki[i], password[i]);
		_fputchar('\n');
		cotnum(di[i], password[i]);
		fclose(password[i]);
	}
	mirexit();
	return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
int filedecode(char password[][200], int n ) {
	miracl *mp = mirsys(10000, 10);
	big ki[n_Max];
	big di[n_Max];
	big plaintextnum = mirvar(0);
	char plaintextfilename[] = "./plaintext.txt";
	for (int i = 0; i < n; i++)
	{
		FILE* cihperfp = fopen(password[i], "r");
		if (cihperfp == NULL) {
			cout << "文件" << password[i] << "打开失败!";
			mirexit();
			return -1;
		}
		ki[i] = mirvar(0);
		di[i] = mirvar(0);
		cinnum(ki[i], cihperfp);
		cinnum(di[i], cihperfp);
		fclose(cihperfp);
	}
	//cout << "请输入输出文件：" << endl;
	if (decode(ki, di, plaintextnum, n) == -1) {
		cout << "解密过程出现错误！" << endl;
		mirexit();
		return -1;
	}
	FILE *plaintextfp = fopen(plaintextfilename, "w+");
	cotnum(plaintextnum, plaintextfp);
	fclose(plaintextfp);
	cout << "文件解密成功！如结果不符合预期，请检查密文文件输入数量是否足够" << endl;
	mirexit();
	return 0;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
int main() {
	int sign;
	cout << "请选择程序运行模式，";
	cout << "1：自动，2：从文件加密并输入密文到文件";
	cout << " 3：从文件解密并输出解密结果到文件" << endl;
	cin >> sign;
	if (sign == 1) {
		int t = 3, n = 5;
		everyauto(t, n);
		system("pause");
		return 0;
	}
	else if (sign == 2) {
		int t, n;
		cout << "请输入加密门限t和n,";
		cout << "t范围不大于" << t_Max;
		cout << ",n范围不大于" << n_Max;
		cout << ":";
		cin >> t >> n;
		cout << endl;
		if (t > t_Max || n > n_Max) {
			cout << "t或n输入过大!" << endl;
			return -1;
		}
		else if (t > n) {
			cout << "错误!t输入不能大于n" << endl;
		}
		int tmp = fileencode(t, n);
		if (tmp == 0) {
			cout << "加密成功！" << endl;
			cout << "该文件若要解密，须同时输入" << t;
			cout << "个密文文件。" << endl;
		}
		system("pause");
		return 0;
	}
	else if (sign == 3) {
		char ciphertextfile[n_Max][200] = { '\0' };
		char InputOrNot[] = "yes";
		/*char forcompare[] = ".txt";
		char tmp[200];*/
		int n;   //计数输入文件数
		for (n = 0; InputOrNot[0] == 'Y'||InputOrNot[0] == 'y'; ++n) {
			cout << "请输入密文文件:" << endl;
			cin >> ciphertextfile[n];
			cout << "是否继续输入文件?(y/n）" << endl;
			cin >> InputOrNot[0];
		}
		if (filedecode(ciphertextfile, n) == -1) {
			system("pause");
			return -1;
		}
		system("pause");
		return 0;
	}
	else {
		cout << "erro:非法输入！" << endl;
		system("pause");
		return -1;
	}
}
