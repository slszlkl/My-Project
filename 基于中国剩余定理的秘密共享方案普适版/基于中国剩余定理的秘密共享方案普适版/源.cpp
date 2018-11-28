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
	for (int i = 0; i < t; i++) //������������ϲ�Ϊһ����ki��ǰ��di�ں�����ⷽ�̺���
	{
		kd[i] = mirvar(0);
		kd[i + t] = mirvar(0);
		int tmp = keynumber[i];
		copy(ki[tmp], kd[i]);
		copy(di[tmp], kd[i + t]);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////
void BigArrayAdds(big a[], big sum ,int n) { //������������
	add(a[0], a[1], sum);										
	for (int i = 0; i < n - 2; i++)
	{
		add(sum, a[i + 2], sum);								
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////
void BigArrayMutlipies(big a[], big M/*result*/, int n) {  //������������
	multiply(a[0], a[1], M);
	for (int r = 0; r < n - 2; r++) {
		multiply(M, a[r+2], M);								
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////
int congruent_equation(big am[], big result, int t) { //�й�ʣ�ඨ���ͬ�෽����
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
	divide(result, M, M);											//resultȡģ
	return 0;
}
///////////////////////////////////////////////////////////////////////////////////////////


int encode(big ki[], big di[], int t, int n, big k/*��֪����*/, big N, big M) { //���ܺ���
	big tmp = mirvar(0);
	big con = mirvar(1);
	char uselessstring[9999] = { '\0' };
	int klen = cotstr(k, uselessstring);
	int klen_devide_n = klen / t;
	if (klen%t != 0) klen_devide_n += 1;
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

/////////////////////////////////////////////////////////////////////////////////////////////
int decode(big ki[], big di[], big k_, int keymuch) {    //����
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
int decodeauto(big k_, big di[], big ki[], int t, int n) {   //ȫ�Զ�ģʽ�µĽ���
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
	if (congruent_equation(kianddiinuse, k_, t) == 0)
		return 0;
	else return -1;
}

////////////////////////////////////////////////////////////////////////////////////////////////
int everyauto(int t, int n) {  //ȫ�Զ�ģʽ���������ҵ�����û���ʾ�ã��������ļ�
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
	big k_ = mirvar(0); //����
	if (decodeauto(k_, di, ki, t, n) == -1) {
		cout << "���ܹ���ʧ�ܣ�" << endl;
		return -1;
	}
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

//////////////////////////////////////////////////////////////////////////////////////////////////
int fileencode(int t, int n) {  //�Դ���һ���������ļ����м��ܲ�����n�������ļ�
	miracl *mip = mirsys(10000, 10);
	char plaintextfile[200];
	cout << "���������ļ���.txt��ʽ������·��:";
	cin >> plaintextfile;
	cout << endl;
	char houzhui[] = ".txt";
	char *locate = strstr(plaintextfile, houzhui);
	if (locate == NULL) {
		cout << "�ļ���ʽ����!" << endl;
		mirexit();
		return -1;
	}
	FILE *fp;
	fp = fopen(plaintextfile, "r");
	if(fp==NULL) {
		cout << "�ļ����뷢������" << endl;
		mirexit();
		return -1;
	};
	big plaintextnum = mirvar(0);
	cinnum(plaintextnum, fp);
	fclose(fp);
	////////////////////�����ļ��������
	 //Ԥ�������ԭʼ�ļ�·��
	int houzhuilocate = locate - plaintextfile; 
	char cipherfileend[] = "_ciphertext01.txt";
	char ciphertextfile[200] = { '\0' };
	strncpy(ciphertextfile, plaintextfile, houzhuilocate);
	strcat(ciphertextfile, cipherfileend);
	cout << "�����У����Ե�..." << endl;
	//����ĵ�һ���ļ���ȷ������ʼ������
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
			cout << "�ļ�" << password[i] << "��ʧ��!";
			mirexit();
			return -1;
		}
		ki[i] = mirvar(0);
		di[i] = mirvar(0);
		cinnum(ki[i], cihperfp);
		cinnum(di[i], cihperfp);
		fclose(cihperfp);
	}
	//cout << "����������ļ���" << endl;
	if (decode(ki, di, plaintextnum, n) == -1) {
		cout << "���ܹ��̳��ִ���" << endl;
		mirexit();
		return -1;
	}
	FILE *plaintextfp = fopen(plaintextfilename, "w+");
	cotnum(plaintextnum, plaintextfp);
	fclose(plaintextfp);
	cout << "�ļ����ܳɹ�������������Ԥ�ڣ����������ļ����������Ƿ��㹻" << endl;
	mirexit();
	return 0;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
int main() {
	int sign;
	cout << "��ѡ���������ģʽ��";
	cout << "1���Զ���2�����ļ����ܲ��������ĵ��ļ�";
	cout << " 3�����ļ����ܲ�������ܽ�����ļ�" << endl;
	cin >> sign;
	if (sign == 1) {
		int t = 3, n = 5;
		everyauto(t, n);
		system("pause");
		return 0;
	}
	else if (sign == 2) {
		int t, n;
		cout << "�������������t��n,";
		cout << "t��Χ������" << t_Max;
		cout << ",n��Χ������" << n_Max;
		cout << ":";
		cin >> t >> n;
		cout << endl;
		if (t > t_Max || n > n_Max) {
			cout << "t��n�������!" << endl;
			return -1;
		}
		else if (t > n) {
			cout << "����!t���벻�ܴ���n" << endl;
		}
		int tmp = fileencode(t, n);
		if (tmp == 0) {
			cout << "���ܳɹ���" << endl;
			cout << "���ļ���Ҫ���ܣ���ͬʱ����" << t;
			cout << "�������ļ���" << endl;
		}
		system("pause");
		return 0;
	}
	else if (sign == 3) {
		char ciphertextfile[n_Max][200] = { '\0' };
		char InputOrNot[] = "yes";
		/*char forcompare[] = ".txt";
		char tmp[200];*/
		int n;   //���������ļ���
		for (n = 0; InputOrNot[0] == 'Y'||InputOrNot[0] == 'y'; ++n) {
			cout << "�����������ļ�:" << endl;
			cin >> ciphertextfile[n];
			cout << "�Ƿ���������ļ�?(y/n��" << endl;
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
		cout << "erro:�Ƿ����룡" << endl;
		system("pause");
		return -1;
	}
}
