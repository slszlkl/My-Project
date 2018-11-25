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


void BigArrayTwotoOne(big ki[], big di[], big keyd[], int t, int keynumber[]) {
	for (int i = 0; i < t; i++) //������������ϲ�Ϊһ����ki��ǰ��di�ں�����ⷽ�̺���
	{
		keyd[i] = mirvar(0);
		keyd[i + t] = mirvar(0);
		int tmp = keynumber[i];
		copy(ki[tmp], keyd[i]);
		copy(di[tmp], keyd[i + t]);
	}
}


void BigArrayAdds(big a[], big sum ,int n) {
	add(a[0], a[1], sum);										
	for (int i = 0; i < n - 2; i++)
	{
		add(sum, a[i + 2], sum);								
	}
}


void BigArrayMutlipies(big a[], big M/*result*/, int n) {
	multiply(a[0], a[1], M);
	for (int r = 0; r < n - 2; r++) {
		multiply(M, a[r+2], M);								
	}
}


int congruent_equation(big am[], big result, int t) {
	big con = mirvar(1);
	big z = mirvar(0);
	int Size = t;
	int DoubleSize = 2 * t;
	for (int i = 0; i < Size; i++) {
		int tmp = (i + Size + 1) % (DoubleSize);
		if (tmp == 0) tmp = Size;
		egcd(am[i + Size], am[tmp], z);
		if (mr_compare(z, con) != 0) {
			cout << "����ֱ�������й�ʣ�ඨ��" << endl;
			system("pause");
			return -1;
		}
	}
	big M = mirvar(0);												//m
	big monij[t_Max];												//Mj^-1
	big Mj[t_Max];													//Mj=m/mi
	big xj[t_Max];													//xj=Mj*Mj^-1*ai mod m

	BigArrayMutlipies(&am[Size], M, Size);

	for (int i = 0; i < Size; i++) {
		Mj[i] = mirvar(0);
		monij[i] = mirvar(0);
		xj[i] = mirvar(0);

		big Mtmp = mirvar(0);
		copy(M, Mtmp);
		divide(Mtmp, am[Size + i], Mj[i]);							//Mj=m/mj
		xgcd(Mj[i], am[i + Size], monij[i], monij[i], monij[i]);		//monij=Mj^-1=Mj^-1 mod mi		
		multiply(Mj[i], am[i], xj[i]);
		multiply(xj[i], monij[i], xj[i]);							//xj[i] = mj*mj^-1*aj		               
		divide(xj[i], M, M);										//xj=xj mod M
	}

	BigArrayAdds(xj, result, Size);
	BOOL sign = divisible(result,M);
	divide(result, M, M);											//resultȡģ
	return 0;
}
///////////////////////////////////////////////////////////////////////////////////////////


int encode(big ki[], big di[], int t, int n, big k/*��֪����*/, big N, big M) {
	big tmp = mirvar(0);
	big con = mirvar(1);
	char uselessstring[9999] = { '\0' };
	int klen = cotstr(k, uselessstring);
	int klen_devide_n = klen / t;
	if (klen%t != 0) klen_devide_n = +1;
	irand((unsigned)time(0));
	for (int i = 0; i < n; i++) {     //ѡ����ͬλ������������������
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


int normaldecode(big ki[], big di[], big k_, int keymuch) {
	int keynumber[n_Max];
	for (int i = 0; i < keymuch; i++)
	{
		keynumber[i] = i;
	}
	big kianddiinuse[2 * t_Max];
	BigArrayTwotoOne(ki, di, kianddiinuse, keymuch, keynumber);
	congruent_equation(kianddiinuse, k_, keymuch);
	return 0;
}


int autodecode(big k_, big di[], big ki[], int t, int n) {
	int keynumber[t_Max];
	for (int i = 0; i < t; i++) {  //ѡ����t����Կ���ڽ���
LOOP2:
		srand((unsigned)time(NULL));
		keynumber[i] = rand() % n;
		for (int r = 0; r < i; r++) {
			if (keynumber[i] == keynumber[r]) goto LOOP2;
		}
	}

	big kianddiinuse[2*t_Max];
	BigArrayTwotoOne(ki, di, kianddiinuse, t, keynumber);
	congruent_equation(kianddiinuse, k_, t);
	return 0;
}


int everyauto(int t, int n) {
	miracl *mip = mirsys(10000, 10);
	big N = mirvar(0);
	big M = mirvar(0);
	big k = mirvar(0);//����k
	irand((unsigned)time(0));
	bigdig(300, 10, k);
	cout << "����k����" << endl;
	cotnum(k, stdout);
	big di[n_Max];
	big ki[n_Max];
	for (int i = 0; i < n; i++)
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
	big k_ = mirvar(0); //����
	autodecode(k_, di, ki, t, n);
	cout << "���ܺ�Ľ��Ϊ:";
	cotnum(k_, stdout);
	cout << endl;
	if (mr_compare(k, k_) == 0) {
		cout << "��mr_compare()�������飬�����������ͬ��" << endl;
		mirexit();
		return 0;
	}
	else {
		cout << "��mr_compare()�������飬��������Ĳ�ͬ����������ʧ�ܡ�" << endl;
		mirexit();
		return -1;
	}
}


int onlyencodefile(FILE *fp, int t, int n) {
	miracl *mip = mirsys(10000, 10);
	big plaintext = mirvar(0);
	cinnum(plaintext, fp);
	char   ciphertext[20][200] = { '\0' };

}


int main() {
	int sign;
	cout << "��ѡ���������ģʽ��";
	cout << "1���Զ���2�����ļ����ܲ��������ĵ��ļ�";
	cout << " 3�����ļ����ܲ�������ܽ�����ļ�" << endl;
	cin >> sign;
	if (sign != 1 && sign != 2 && sign != 3) {
		cout << "erro:�Ƿ����룡" << endl;
		system("pause");
		return -1;
	}
	else if (sign == 1) {
		int t = 3, n = 5;
		everyauto(t, n);
		system("pause");
		return 0;
	}
	else if (sign == 2) {
		char plaintextfile[200];
		cout << "�����ļ�����·��:";
		cin >> plaintextfile;
		cout << endl;
		FILE *fp;
		fp = fopen(plaintextfile, "r+");
		if (fp != NULL) {
			int t, n;
			cout << "�������������t��n:";
			cin >> t >> n;
			cout << endl;
			if (t > t_Max || n > n_Max) {
				cout << "t��n�������!" << endl;
				return -1;
			}
			onlyencodefile(fp, t, n);
		} 
		else {
			cout << "�ļ����뷢������" << endl;
			system("pause");
			return -1;
		}
		fclose(fp);
		system("pause");
		return 0;
	}
}
