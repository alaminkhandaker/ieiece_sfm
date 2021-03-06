#include <stdio.h>
#include <string.h>
#include <gmp.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>

#define TRUE 1
#define FALSE 0
#define c1 2

struct Fp{
	mpz_t x0;
};
struct Fp3{
	struct Fp x0,x1,x2;
};
struct Fp6{
	struct Fp3 x0,x1;
};
struct Fp18{
	struct Fp6 x0,x1,x2;
};
struct EFp{
	struct Fp x,y;
	int infity;
};
struct EFp3{
	struct Fp3 x,y;
	int infity;
};
struct EFp6{
	struct Fp6 x,y;
	int infity;
};
struct EFp18{
	struct Fp18 x,y;
	int infity;
};
mpz_t X;

mpz_t prime,order_r,trace_t, order_EFp, b;
struct EFp18 G2Q,phiQ,PscmQ,Q2,Q1,ZQ,ZQ1,ZQ2,QinG2;
struct EFp18 QQ1,QQ2,Q1Q2,QQ1Q2,ZQZQ1,ZQZQ2,ZQ1ZQ2,ZQZQ1ZQ2;

struct EFp3 Q_d,Q1_d,Q2_d,ZQ_d,ZQ1_d,ZQ2_d;
struct EFp3 QQ1_d,QQ2_d,Q1Q2_d,QQ1Q2_d,ZQZQ1_d,ZQZQ2_d,ZQ1ZQ2_d,ZQZQ1ZQ2_d;

struct Fp v1_pre_com, w1_pre_com;
struct Fp3 i,i_inv,i_precom;

mpz_t x1,S1X,S0X,S2X,S3X,S4X,S5X;
mpz_t s0,s1,s2,s3,s4,s5;

void EFp_SCM_ML(struct EFp *ANS, struct EFp *P,mpz_t j);
void EFp3_SCM_ML(struct EFp3 *ANS, struct EFp3 *P,mpz_t scalar);
void EFp6_SCM_ML(struct EFp6 *ANS, struct EFp6 *P,mpz_t scalar);
void EFp18_SCM_ML(struct EFp18 *ANS, struct EFp18 *P,mpz_t scalar);


#pragma mark Fp methods
void Fp_init(struct Fp *A);
void Fp_set(struct Fp *ANS,struct Fp *A);
void Fp_set_ui(struct Fp *A,signed long int B);
void Fp_random(struct Fp *A);
void Fp_clear(struct Fp *A);
void Fp_printf(struct Fp *A);
void Fp_add(struct Fp *ans,struct Fp *a,struct Fp *b);//ans=a+b mod p
void Fp_add_ui(struct Fp *ans,struct Fp *a,unsigned long int b);//ans=a+b mod p
void Fp_sub(struct Fp *ans,struct Fp *a,struct Fp *b);//ans=a-b mod p
void Fp_sub_ui(struct Fp *ans,struct Fp *a,unsigned long int b);//ans=a+b mod p
void Fp_mul(struct Fp *ans,struct Fp *a,struct Fp *b);//ans=a*b mod p
void Fp_mul_ui(struct Fp *ans,struct Fp *a,unsigned long int b);//ans=a*b mod p
void Fp_div(struct Fp *ans,struct Fp *a,struct Fp *b);//ans=a/b mod p
void Fp_pow(struct Fp *ans,struct Fp *a,mpz_t b);
void Fp_sqrt(struct Fp *ans,struct Fp *a);//x^2=a mod p
int  Fp_cmp_mpz(struct Fp *A,mpz_t B);
void Fp_mul_mpz(struct Fp *ANS,struct Fp *A,mpz_t B);

//-----------------------------------------------------------------------------------------
#pragma mark Fp3 methods
void Fp3_init(struct Fp3 *A);
void Fp3_set(struct Fp3 *ANS,struct Fp3 *A);
void Fp3_set_ui(struct Fp3 *A,signed long int B);
void Fp3_random(struct Fp3 *A);
void Fp3_clear(struct Fp3 *A);
void Fp3_printf(struct Fp3 *A);
void Fp3_add(struct Fp3 *ANS,struct Fp3 *A,struct Fp3 *B);
void Fp3_add_ui(struct Fp3 *ANS,struct Fp3 *A,unsigned long int B);
void Fp3_sub(struct Fp3 *ANS,struct Fp3 *A,struct Fp3 *B);
void Fp3_mul(struct Fp3 *ANS,struct Fp3 *A,struct Fp3 *B);
void Fp3_mul_xi(struct Fp3 *ANS,struct Fp3 *A);
void Fp3_mul_ui(struct Fp3 *ANS,struct Fp3 *A,unsigned long int B);
void Fp3_mul_Fp(struct Fp3 *ANS,struct Fp3 *A,struct Fp *B);
void Fp3_neg(struct Fp3 *ANS,struct Fp3 *A);
void Fp3_invert(struct Fp3 *ANS,struct Fp3 *A);
void Fp3_div(struct Fp3 *ANS,struct Fp3 *A,struct Fp3 *B);
void Fp3_pow(struct Fp3 *ANS,struct Fp3 *A,mpz_t B);
void Fp3_sqrt(struct Fp3 *ANS,struct Fp3 *A);//x^2=a mod p
int  Fp3_legendre(struct Fp3 *A);
int  Fp3_cmp(struct Fp3 *A,struct Fp3 *B);
int  Fp3_cmp_mpz(struct Fp3 *A,mpz_t B);
void Fp3_neg(struct Fp3 *ANS,struct Fp3 *A);

//-----------------------------------------------------------------------------------------
#pragma mark Fp6 methods
void Fp6_init(struct Fp6 *A);
void Fp6_set(struct Fp6 *ANS,struct Fp6 *A);
void Fp6_set_ui(struct Fp6 *A,signed long int B);
void Fp6_random(struct Fp6 *A);
void Fp6_clear(struct Fp6 *A);
void Fp6_printf(struct Fp6 *A);
void Fp6_add(struct Fp6 *ANS,struct Fp6 *A,struct Fp6 *B);
void Fp6_add_ui(struct Fp6 *ANS,struct Fp6 *A,unsigned long int B);
void Fp6_sub(struct Fp6 *ANS,struct Fp6 *A,struct Fp6 *B);
void Fp6_mul(struct Fp6 *ANS,struct Fp6 *A,struct Fp6 *B);
void Fp6_mul_v(struct Fp6 *ANS,struct Fp6 *A);
void Fp6_mul_ui(struct Fp6 *ANS,struct Fp6 *A,unsigned long int B);
void Fp6_mul_Fp(struct Fp6 *ANS,struct Fp6 *A,struct Fp *B);
void Fp6_neg(struct Fp6 *ANS,struct Fp6 *A);
void Fp6_invert(struct Fp6 *ANS,struct Fp6 *A);
void Fp6_div(struct Fp6 *ANS,struct Fp6 *A,struct Fp6 *B);
void Fp6_pow(struct Fp6 *ANS,struct Fp6 *A,mpz_t B);
void Fp6_sqrt(struct Fp6 *ANS,struct Fp6 *A);//x^2=a mod p
int  Fp6_legendre(struct Fp6 *A);
int  Fp6_cmp(struct Fp6 *A,struct Fp6 *B);
int  Fp6_cmp_mpz(struct Fp6 *A,mpz_t B);
void Fp6_neg(struct Fp6 *ANS,struct Fp6 *A);

//-----------------------------------------------------------------------------------------
#pragma mark Fp18 methods
void Fp18_init(struct Fp18 *A);
void Fp18_set(struct Fp18 *ANS,struct Fp18 *A);
void Fp18_set_ui(struct Fp18 *A,signed long int B);
void Fp18_random(struct Fp18 *A);
void Fp18_clear(struct Fp18 *A);
void Fp18_printf(struct Fp18 *A);
void Fp18_add(struct Fp18 *ANS,struct Fp18 *A,struct Fp18 *B);
void Fp18_add_ui(struct Fp18 *ANS,struct Fp18 *A,unsigned long int B);
void Fp18_sub(struct Fp18 *ANS,struct Fp18 *A,struct Fp18 *B);
void Fp18_mul(struct Fp18 *ANS,struct Fp18 *A,struct Fp18 *B);
void Fp18_mul_ui(struct Fp18 *ANS,struct Fp18 *A,unsigned long int B);
void Fp18_mul_Fp(struct Fp18 *ANS,struct Fp18 *A,struct Fp *B);
void Fp18_neg(struct Fp18 *ANS,struct Fp18 *A);
void Fp18_invert(struct Fp18 *ANS,struct Fp18 *A);
void Fp18_div(struct Fp18 *ANS,struct Fp18 *A,struct Fp18 *B);
void Fp18_pow(struct Fp18 *ANS,struct Fp18 *A,mpz_t B);
void Fp18_sqrt(struct Fp18 *ANS,struct Fp18 *A);//x^2=a mod p
int  Fp18_legendre(struct Fp18 *A);
int  Fp18_cmp(struct Fp18 *A,struct Fp18 *B);
int  Fp18_cmp_mpz(struct Fp18 *A,mpz_t B);
void Fp18_neg(struct Fp18 *ANS,struct Fp18 *A);
void Fp18_frobenius_map(struct Fp18 *ANS, struct Fp18 *A);

//-----------------------------------------------------------------------------------------
#pragma mark EFp methods
void EFp_init(struct EFp *A);
void EFp_set(struct EFp *A,struct EFp *B);
void EFp_set_infity(struct EFp *A);
void EFp_clear(struct EFp *A);
void EFp_printf(struct EFp *A);
void EFp_SCM_BIN(struct EFp *ANS, struct EFp *P,mpz_t j);
void EFp_SCM_NAF(struct EFp *ANS, struct EFp *P, mpz_t scalar);
void EFp_SCM_WIN(struct EFp *ANS, struct EFp *P, mpz_t scalar);
void EFp_ECD(struct EFp *ANS, struct EFp *P);//ANS=2*P
void EFp_ECA(struct EFp *ANS, struct EFp *P1, struct EFp *P2);//ANS=P1+P2
int  EFp_cmp(struct EFp *A,struct EFp *B);
void EFp_random_set(struct EFp *ANS);//random set EFp on curve
void EFp_neg(struct EFp *ANS, struct EFp *A);

//-----------------------------------------------------------------------------------------
#pragma mark EFp3 methods
void EFp3_init(struct EFp3 *A);
void EFp3_set(struct EFp3 *A,struct EFp3 *B);
void EFp3_set_infity(struct EFp3 *A);
void EFp3_clear(struct EFp3 *A);
void EFp3_printf(struct EFp3 *A);
void EFp3_ECD(struct EFp3 *ANS, struct EFp3 *P);//ANS=2*P
void EFp3_ECA(struct EFp3 *ANS, struct EFp3 *P1, struct EFp3 *P2);//ANS=P1+P2
int  EFp3_cmp(struct EFp3 *A,struct EFp3 *B);
void EFp3_set_EFp(struct EFp3 *A,struct EFp *B);
void EFp3_random_set(struct EFp3 *ANS);
void EFp3_SCM_BIN(struct EFp3 *ANS, struct EFp3 *P, mpz_t j);
void EFp3_SCM_NAF(struct EFp3 *ANS, struct EFp3 *P, mpz_t scalar);
void EFp3_SCM_WIN(struct EFp3 *ANS, struct EFp3 *P, mpz_t scalar);
void EFp3_neg(struct EFp3 *ANS, struct EFp3 *A);

//-----------------------------------------------------------------------------------------
#pragma mark EFp6 methods
void EFp6_init(struct EFp6 *A);
void EFp6_set(struct EFp6 *A,struct EFp6 *B);
void EFp6_set_infity(struct EFp6 *A);
void EFp6_clear(struct EFp6 *A);
void EFp6_printf(struct EFp6 *A);
void EFp6_ECD(struct EFp6 *ANS, struct EFp6 *P);//ANS=2*P
void EFp6_ECA(struct EFp6 *ANS, struct EFp6 *P1, struct EFp6 *P2);//ANS=P1+P2
int  EFp6_cmp(struct EFp6 *A,struct EFp6 *B);
void EFp6_random_set(struct EFp6 *ANS);
void EFp6_SCM_BIN(struct EFp6 *ANS, struct EFp6 *P, mpz_t j);
void EFp6_SCM_NAF(struct EFp6 *ANS, struct EFp6 *P, mpz_t scalar);
void EFp6_SCM_WIN(struct EFp6 *ANS, struct EFp6 *P, mpz_t scalar);
void EFp6_neg(struct EFp6 *ANS, struct EFp6 *A);
//-----------------------------------------------------------------------------------------
#pragma mark EFp18 methods
void EFp18_init(struct EFp18 *A);
void EFp18_set(struct EFp18 *A,struct EFp18 *B);
void EFp18_set_infity(struct EFp18 *A);
void EFp18_clear(struct EFp18 *A);
void EFp18_printf(struct EFp18 *A);
void EFp18_ECD(struct EFp18 *ANS, struct EFp18 *P);//ANS=2*P
void EFp18_ECA(struct EFp18 *ANS, struct EFp18 *P1, struct EFp18 *P2);//ANS=P1+P2
int  EFp18_cmp(struct EFp18 *A,struct EFp18 *B);
void EFp18_random_set(struct EFp18 *ANS);
void EFp18_random_set_for_Ate(struct EFp18 *ANS);
void EFp18_SCM_BIN(struct EFp18 *ANS, struct EFp18 *P, mpz_t j);
void EFp18_SCM_NAF(struct EFp18 *ANS, struct EFp18 *P, mpz_t scalar);
void EFp18_SCM_WIN(struct EFp18 *ANS, struct EFp18 *P, mpz_t scalar);
void EFp18_frobenius_map(struct EFp18 *ANS,struct EFp18 *A);
void EFp18_neg(struct EFp18 *ANS, struct EFp18 *A);
//-----------------------------------------------------------------------------------------

#pragma mark Parameters methods
void EFp_set_EC_parameter(void);
void get_precalc_frobenious_mapped_points_in_EFp18(struct EFp18 *ANS);
void EFp18_SCM_New(struct EFp18 *ANS, struct EFp18 *P, mpz_t S);
void EFp3_SCM_New(struct EFp3 *ANS, struct EFp3 *P, mpz_t S);
void Check_SCM(struct EFp18 *ANS);
float timedifference_msec(struct timeval t0, struct timeval t1);
void get_z_adic(mpz_t S);

void EFp18_to_EFp3_map(struct EFp3 *ANS,struct EFp18 *A);
void EFp3_to_EFp18_map(struct EFp18 *ANS,struct EFp3 *A);
void Skew_Frobenius_map(struct EFp3 *ANS, struct EFp3 *Qt);
void get_precalc_skew_frobenious_mapped_points_in_EFp3(struct EFp3 *ANS);
void pre_calc();


#pragma mark Pairing methods
void Miller_algo(struct Fp18 *ANS,struct EFp18 *P,struct EFp18 *Q,mpz_t roop);
void Optimal_Miller_algo(struct Fp18 *ANS,struct EFp18 *P,struct EFp18 *Q,mpz_t roop);
void Final_Exp(struct Fp18 *ANS,struct Fp18 *A);
void Tate_Pairing(struct Fp18 *ANS,struct EFp18 *P,struct EFp18 *Q);
void Ate_Pairing(struct Fp18 *ANS,struct EFp18 *P,struct EFp18 *Q);
void Optimal_Ate_Pairing(struct Fp18 *ANS,struct EFp18 *G1,struct EFp18 *G2);
void ltt_q(struct Fp18 *ANS,struct EFp18 *T,struct EFp18 *Q);
void v2t_q(struct Fp18 *ANS,struct EFp18 *T,struct EFp18 *Q);
void ltp_q(struct Fp18 *ANS,struct EFp18 *T,struct EFp18 *P,struct EFp18 *Q);
void vtp_q(struct Fp18 *ANS,struct EFp18 *T,struct EFp18 *Q);
void check_Pairing(void);
