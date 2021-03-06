
#include "embedding_degree18.h"

int count_eca_new, count_ecd_new,count_eca_BIN, count_ecd_BIN,count_eca_BIN_map, count_ecd_BIN_map, count_eca_WIN, count_ecd_WIN, count_eca_WIN_map,count_ecd_WIN_map, count_eca_NAF, count_ecd_NAF,count_eca_NAF_map,count_ecd_NAF_map, count_eca_ML, count_ecd_ML,count_eca_ML_map, count_ecd_ML_map;
int count_eca_new_pre, count_ecd_new_pre;
long int myNAF[5000];
long int naf_index = 0;

struct timeval tonaf_t0;
struct timeval tonaf_t1;
float elapsed_tonaf_t0;
float elapsed_tonaf_t1;

int main(void){
    mpz_init(X);
    
    //c1=2
    mpz_set_str(X,"21118454",10);//191bit
    mpz_set_str(X,"23058430092138432950",10);//r=

    
    mpz_init(prime);
    mpz_init(order_r);
    mpz_init(order_EFp);
    mpz_init(trace_t);
    mpz_init(b);
    
    EFp18_init(&G2Q);
    EFp18_init(&QinG2);
    EFp18_init(&phiQ);
    EFp18_init(&PscmQ);
    
    EFp_set_EC_parameter();
    
    gmp_printf("p = %Zd\n",prime);
    gmp_printf("r = %Zd\n",order_r);
    gmp_printf("t = %Zd\n",trace_t);
    gmp_printf("#E(Fp) = %Zd\n",order_EFp);
    
    printf("X = %dbit\n",(int)mpz_sizeinbase(X,2));
    printf("p = %dbit\n",(int)mpz_sizeinbase(prime,2));
    printf("r = %dbit\n",(int)mpz_sizeinbase(order_r,2));
    printf("t = %dbit\n",(int)mpz_sizeinbase(trace_t,2));
    
    gmp_printf("y^2 = x^3 + %Zd\n",b);
    
    pre_calc();
    check_Pairing();
    struct EFp18 ANS,AA;
    EFp18_init(&ANS);
//    EFp18_init(&AA);
//    EFp18_random_set_for_Ate(&ANS);
//    EFp18_SCM_BIN(&AA , &ANS, order_r);
//    EFp18_printf(&AA);
//    Check_SCM(&AA);
    
    mpz_clear(prime);
    mpz_clear(order_r);
    mpz_clear(b);
    return 0;
}

//-----------------------------------------------------------------------------------------
#pragma mark Fp methods
void Fp_init(struct Fp *A){
    mpz_init(A->x0);
}
void Fp_set(struct Fp *ANS,struct Fp *A){
    mpz_set(ANS->x0,A->x0);
}
void Fp_set_ui(struct Fp *A,signed long int B){
    mpz_set_ui(A->x0,B);
}
void Fp_random(struct Fp *A){
    mpz_random(A->x0,10);
    mpz_mod(A->x0,A->x0,prime);
}
void Fp_clear(struct Fp *A){
    mpz_clear(A->x0);
}
void Fp_printf(struct Fp *A){
    gmp_printf("%Zd\n",A->x0);
}
void Fp_add(struct Fp *ANS,struct Fp *A,struct Fp *B){
    struct Fp tmp;
    Fp_init(&tmp);
    
    mpz_add(tmp.x0,A->x0,B->x0);
    mpz_mod(tmp.x0,tmp.x0,prime);
    
    Fp_set(ANS,&tmp);
    
    Fp_clear(&tmp);
}
void Fp_add_ui(struct Fp *ANS,struct Fp *A,unsigned long int B){
    struct Fp tmp;
    Fp_init(&tmp);
    
    mpz_add_ui(tmp.x0,A->x0,B);
    mpz_mod(tmp.x0,tmp.x0,prime);
    
    Fp_set(ANS,&tmp);
    Fp_clear(&tmp);
}
void Fp_sub(struct Fp *ANS,struct Fp *A,struct Fp *B){
    struct Fp tmp;
    Fp_init(&tmp);
    
    mpz_sub(tmp.x0,A->x0,B->x0);
    mpz_mod(tmp.x0,tmp.x0,prime);
    
    Fp_set(ANS,&tmp);
    
    Fp_clear(&tmp);
}
void Fp_sub_ui(struct Fp *ANS,struct Fp *A,unsigned long int B){
    struct Fp tmp;
    Fp_init(&tmp);
    
    mpz_sub_ui(tmp.x0,A->x0,B);
    mpz_mod(tmp.x0,tmp.x0,prime);
    
    Fp_set(ANS,&tmp);
    
    Fp_clear(&tmp);
}
void Fp_mul(struct Fp *ANS,struct Fp *A,struct Fp *B){
    struct Fp tmp;
    Fp_init(&tmp);
    
    mpz_mul(tmp.x0,A->x0,B->x0);
    mpz_mod(tmp.x0,tmp.x0,prime);
    
    Fp_set(ANS,&tmp);
    
    Fp_clear(&tmp);
}

void Fp_mul_mpz(struct Fp *ANS,struct Fp *A,mpz_t B){
    struct Fp tmp;
    Fp_init(&tmp);
    
    mpz_mul(tmp.x0,A->x0,B);
    mpz_mod(tmp.x0,tmp.x0,prime);
    
    Fp_set(ANS,&tmp);
    
    Fp_clear(&tmp);
}

void Fp_mul_ui(struct Fp *ANS,struct Fp *A,unsigned long int B){
    struct Fp tmp;
    Fp_init(&tmp);
    
    mpz_mul_ui(tmp.x0,A->x0,B);
    mpz_mod(tmp.x0,tmp.x0,prime);
    
    Fp_set(ANS,&tmp);
    
    Fp_clear(&tmp);
}
void Fp_div(struct Fp *ANS,struct Fp *A,struct Fp *B){
    struct Fp tmp;
    Fp_init(&tmp);
    
    mpz_invert(tmp.x0,B->x0,prime);
    mpz_mul(tmp.x0,A->x0,tmp.x0);
    mpz_mod(tmp.x0,tmp.x0,prime);
    
    Fp_set(ANS,&tmp);
    
    Fp_clear(&tmp);
}
void Fp_pow(struct Fp *ANS,struct Fp *A,mpz_t j){
    int i;
    int r;//bit数
    r= (int)mpz_sizeinbase(j,2);
    //printf("r= %d\n",r);
    
    struct Fp answer_tmp;
    Fp_init(&answer_tmp);
    Fp_set(&answer_tmp,A);
    
    struct Fp in_tmp;
    Fp_init(&in_tmp);
    Fp_set(&in_tmp,A);
    
    for(i=r-2;i>=0;i--){
        if(mpz_tstbit(j,i)==1){
            Fp_mul(&answer_tmp,&answer_tmp,&answer_tmp);//a*2
            Fp_mul(&answer_tmp,&answer_tmp,&in_tmp);//*a
        }else{
            Fp_mul(&answer_tmp,&answer_tmp,&answer_tmp);//a*2
        }
    }
    
    Fp_set(ANS,&answer_tmp);
    
    Fp_clear(&answer_tmp);
    Fp_clear(&in_tmp);
}
void Fp_sqrt(struct Fp *ANS,struct Fp *A){
    struct Fp n_tmp,y_tmp,x_tmp,b_tmp,t_tmp,tmp_Fp;
    Fp_init(&n_tmp);
    Fp_init(&y_tmp);
    Fp_init(&x_tmp);
    Fp_init(&b_tmp);
    Fp_init(&t_tmp);
    Fp_init(&tmp_Fp);
    
    Fp_set(&n_tmp,A);
    
    mpz_t tmp_mpz,q_tmp,e_tmp,r_tmp,set_1,set_2;
    mpz_init(tmp_mpz);
    mpz_init(q_tmp);
    mpz_init(e_tmp);
    mpz_init(r_tmp);
    mpz_init(set_1);
    mpz_init(set_2);
    
    mpz_set_ui(set_1,1);
    mpz_set_ui(set_2,2);
    
    while(mpz_legendre(n_tmp.x0,prime)!=-1){
        Fp_add_ui(&n_tmp,&n_tmp,1);
    }
    
    mpz_set(q_tmp,prime);
    mpz_sub_ui(q_tmp,q_tmp,1);
    mpz_set_ui(e_tmp,0);
    
    while(mpz_odd_p(q_tmp)==0){
        mpz_add_ui(e_tmp,e_tmp,1);
        mpz_div_ui(q_tmp,q_tmp,2);
    }
    
    Fp_pow(&y_tmp,&n_tmp,q_tmp);
    
    mpz_set(r_tmp,e_tmp);
    
    mpz_sub_ui(tmp_mpz,q_tmp,1);
    mpz_div_ui(tmp_mpz,tmp_mpz,2);
    Fp_pow(&x_tmp,A,tmp_mpz);
    Fp_pow(&tmp_Fp,&x_tmp,set_2);
    Fp_mul(&b_tmp,&tmp_Fp,A);
    Fp_mul(&x_tmp,&x_tmp,A);
    
    int m;
    
    while(Fp_cmp_mpz(&b_tmp,set_1)==1){
        m=-1;
        while(Fp_cmp_mpz(&tmp_Fp,set_1)==1){
            m++;
            mpz_pow_ui(tmp_mpz,set_2,m);
            Fp_pow(&tmp_Fp,&b_tmp,tmp_mpz);
        }
        // gmp_printf("%Zd,%Zd\n",x.x0.x0,x.x1.x0);
        mpz_sub_ui(tmp_mpz,r_tmp,m);
        mpz_sub_ui(tmp_mpz,tmp_mpz,1);
        mpz_powm(tmp_mpz,set_2,tmp_mpz,prime);
        Fp_pow(&t_tmp,&y_tmp,tmp_mpz);
        Fp_pow(&y_tmp,&t_tmp,set_2);
        mpz_set_ui(r_tmp,m);
        Fp_mul(&x_tmp,&x_tmp,&t_tmp);
        Fp_mul(&b_tmp,&b_tmp,&y_tmp);
    }
    
    Fp_set(ANS,&x_tmp);
    
    Fp_clear(&n_tmp);
    Fp_clear(&y_tmp);
    Fp_clear(&x_tmp);
    Fp_clear(&b_tmp);
    Fp_clear(&t_tmp);
    Fp_clear(&tmp_Fp);
    mpz_clear(tmp_mpz);
    mpz_clear(q_tmp);
    mpz_clear(e_tmp);
    mpz_clear(r_tmp);
    mpz_clear(set_1);
}
void Fp_neg(struct Fp *ANS,struct Fp *A){
    struct Fp tmp;
    Fp_init(&tmp);
    
    mpz_sub(tmp.x0,prime,A->x0);
    
    Fp_set(ANS,&tmp);
    
    Fp_clear(&tmp);
}
int Fp_cmp(struct Fp *A,struct Fp *B){
    if(mpz_cmp(A->x0,B->x0)==0){
        return 0;
    }
    return 1;
}
int Fp_cmp_mpz(struct Fp *A,mpz_t B){
    if(mpz_cmp(A->x0,B)==0){
        return 0;
    }
    return 1;
}

//-----------------------------------------------------------------------------------------
#pragma mark Fp3 methods
void Fp3_init(struct Fp3 *A){
    Fp_init(&A->x0);
    Fp_init(&A->x1);
    Fp_init(&A->x2);
}
void Fp3_set(struct Fp3 *ANS,struct Fp3 *A){
    Fp_set(&ANS->x0,&A->x0);
    Fp_set(&ANS->x1,&A->x1);
    Fp_set(&ANS->x2,&A->x2);
}
void Fp3_set_ui(struct Fp3 *A,signed long int B){
    Fp_set_ui(&A->x0,B);
    Fp_set_ui(&A->x1,B);
    Fp_set_ui(&A->x2,B);
}
void Fp3_random(struct Fp3 *A){
    Fp_random(&A->x0);
    Fp_random(&A->x1);
    Fp_random(&A->x2);
}
void Fp3_clear(struct Fp3 *A){
    Fp_clear(&A->x0);
    Fp_clear(&A->x1);
    Fp_clear(&A->x2);
}
void Fp3_printf(struct Fp3 *A){
    gmp_printf("%Zd,%Zd,%Zd\n",A->x0.x0,A->x1.x0,A->x2.x0);
}
void Fp3_add(struct Fp3 *ANS,struct Fp3 *A,struct Fp3 *B){
    struct Fp3 tmp;
    Fp3_init(&tmp);
    
    Fp_add(&tmp.x0,&A->x0,&B->x0);
    Fp_add(&tmp.x1,&A->x1,&B->x1);
    Fp_add(&tmp.x2,&A->x2,&B->x2);
    
    Fp3_set(ANS,&tmp);
    
    Fp3_clear(&tmp);
}
void Fp3_add_ui(struct Fp3 *ANS,struct Fp3 *A,unsigned long int B){
    struct Fp3 tmp;
    Fp3_init(&tmp);
    
    Fp_add_ui(&tmp.x0,&A->x0,B);
    Fp_add_ui(&tmp.x1,&A->x1,B);
    Fp_add_ui(&tmp.x2,&A->x2,B);
    
    Fp3_set(ANS,&tmp);
    
    Fp3_clear(&tmp);
}
void Fp3_sub(struct Fp3 *ANS,struct Fp3 *A,struct Fp3 *B){
    struct Fp3 tmp;
    Fp3_init(&tmp);
    
    Fp_sub(&tmp.x0,&A->x0,&B->x0);
    Fp_sub(&tmp.x1,&A->x1,&B->x1);
    Fp_sub(&tmp.x2,&A->x2,&B->x2);
    
    Fp3_set(ANS,&tmp);
    
    Fp3_clear(&tmp);
}
void Fp3_mul(struct Fp3 *ANS,struct Fp3 *A,struct Fp3 *B){
    //(x0,x1,x2)*(y0,y1,y2)=(x0y0+xi((x1+x2)(y1+y2)-x1y1-x2y2),xix2y2+(x0+x1)(y0+y1)-x0y0-x1y1,x1y1+(x0+x2)(y0+y2)-x0y0-x2y2)
    struct Fp tmp00,tmp11,tmp22,tmpx01,tmpx12,tmpx20,tmpy01,tmpy12,tmpy20,t0,t1,t2,tmp;
    struct Fp3 t_ans;
    Fp_init(&tmp00);
    Fp_init(&tmp11);
    Fp_init(&tmp22);
    Fp_init(&tmpx01);
    Fp_init(&tmpx12);
    Fp_init(&tmpx20);
    Fp_init(&tmpy01);
    Fp_init(&tmpy12);
    Fp_init(&tmpy20);
    Fp_init(&t0);
    Fp_init(&t1);
    Fp_init(&t2);
    Fp_init(&tmp);
    Fp3_init(&t_ans);
    
    Fp_mul(&tmp00,&A->x0,&B->x0);//x0*y0
    Fp_mul(&tmp11,&A->x1,&B->x1);//x1*y1
    Fp_mul(&tmp22,&A->x2,&B->x2);//x2*y2
    
    Fp_add(&tmpx01,&A->x0,&A->x1);//x0+x1
    Fp_add(&tmpx12,&A->x1,&A->x2);//x1+x2
    Fp_add(&tmpx20,&A->x0,&A->x2);//x2+x0
    Fp_add(&tmpy01,&B->x0,&B->x1);//y0+y1
    Fp_add(&tmpy12,&B->x1,&B->x2);//y1+y2
    Fp_add(&tmpy20,&B->x0,&B->x2);//y2+y0
    
    Fp_mul(&t0,&tmpx01,&tmpy01);//(x0+x1)(y0+y1)
    Fp_mul(&t1,&tmpx12,&tmpy12);//(x1+x2)(y1+y2)
    Fp_mul(&t2,&tmpx20,&tmpy20);//(x2+x0)(y2+y0)
    //(x0,x1,x2)*(y0,y1,y2)=(x0y0+xi((x1+x2)(y1+y2)-x1y1-x2y2),xix2y2+(x0+x1)(y0+y1)-x0y0-x1y1,x1y1+(x0+x2)(y0+y2)-x0y0-x2y2)
    Fp_sub(&t1,&t1,&tmp11);
    Fp_sub(&t1,&t1,&tmp22);//(x1+x2)(y1+y2)-x1y1-x2y2
    Fp_mul_ui(&tmp,&t1,c1);
    Fp_add(&t_ans.x0,&tmp00,&tmp);
    
    Fp_sub(&t0,&t0,&tmp00);
    Fp_sub(&t0,&t0,&tmp11);
    Fp_mul_ui(&tmp,&tmp22,c1);
    Fp_add(&t_ans.x1,&tmp,&t0);
    
    Fp_sub(&t2,&t2,&tmp00);
    Fp_sub(&t2,&t2,&tmp22);
    Fp_add(&t_ans.x2,&tmp11,&t2);
    
    Fp3_set(ANS,&t_ans);
    
    Fp_clear(&tmp00);
    Fp_clear(&tmp11);
    Fp_clear(&tmp22);
    Fp_clear(&tmpx01);
    Fp_clear(&tmpx12);
    Fp_clear(&tmpx20);
    Fp_clear(&tmpy01);
    Fp_clear(&tmpy12);
    Fp_clear(&tmpy20);
    Fp_clear(&t0);
    Fp_clear(&t1);
    Fp_clear(&t2);
    Fp_clear(&tmp);
}
void Fp3_mul_xi(struct Fp3 *ANS,struct Fp3 *A){
    struct Fp3 tmp;
    Fp3_init(&tmp);
    
    Fp_mul_ui(&tmp.x0,&A->x2,c1);
    Fp_set(&tmp.x1,&A->x0);
    Fp_set(&tmp.x2,&A->x1);
    
    Fp3_set(ANS,&tmp);
    
    Fp3_clear(&tmp);
}
void Fp3_mul_ui(struct Fp3 *ANS,struct Fp3 *A,unsigned long int B){
    struct Fp3 tmp;
    Fp3_init(&tmp);
    
    Fp_mul_ui(&tmp.x0,&A->x0,B);
    Fp_mul_ui(&tmp.x1,&A->x1,B);
    Fp_mul_ui(&tmp.x2,&A->x2,B);
    
    Fp3_set(ANS,&tmp);
    Fp3_clear(&tmp);
}
void Fp3_mul_Fp(struct Fp3 *ANS,struct Fp3 *A,struct Fp *B){
    struct Fp3 tmp;
    Fp3_init(&tmp);
    
    Fp_mul(&tmp.x0,&A->x0,B);
    Fp_mul(&tmp.x1,&A->x1,B);
    Fp_mul(&tmp.x2,&A->x2,B);
    
    Fp3_set(ANS,&tmp);
    
    Fp3_clear(&tmp);
}
void Fp3_invert(struct Fp3 *ANS, struct Fp3 *A){
    struct Fp3 t_ans;
    Fp3_init(&t_ans);
    
    struct Fp T0,T1,t0,t1,t2,t3;
    Fp_init(&T0);
    Fp_init(&T1);
    Fp_init(&t0);
    Fp_init(&t1);
    Fp_init(&t2);
    Fp_init(&t3);
    
    // An optimized version of Grewal's Algo. 3   (a,b,c)
    Fp_mul(&T0,&A->x0,&A->x0);
    Fp_mul_ui(&t0,&A->x1,c1);
    
    Fp_mul(&T1,&t0,&A->x2);
    Fp_sub(&t1,&T0,&T1); // t1=(a^2-bci) mod q
    
    Fp_mul(&T0,&A->x2,&A->x2);
    Fp_mul_ui(&T0,&T0,c1);
    Fp_mul(&T1,&A->x0,&A->x1);
    Fp_sub(&t2,&T0,&T1); // t2=(c^2i-ab) mod q
    
    Fp_mul(&T0,&A->x1,&A->x1);
    Fp_mul(&T1,&A->x0,&A->x2);
    Fp_sub(&t3,&T0,&T1); // t3=(b^2-ac) mod q
    
    Fp_mul(&T0,&t0,&t3);
    Fp_mul(&T1,&A->x0,&t1);
    Fp_add(&T0,&T0,&T1); // T0={bi(b^2-ac)+a(a^2-bci)} mod q
    
    Fp_mul_ui(&t0,&A->x2,c1);
    Fp_mul(&T1,&t0,&t2);
    Fp_add(&t0,&T0,&T1); // t0={ci(c^2i-ab)+{bi(b^2-ac)+a(a^2-bci)}} mod q .0
    
    mpz_invert(t0.x0,t0.x0,prime);
    
    Fp_mul(&t_ans.x0,&t1,&t0);
    Fp_mul(&t_ans.x1,&t2,&t0);
    Fp_mul(&t_ans.x2,&t3,&t0);
    
    Fp3_set(ANS,&t_ans);
    
    Fp3_clear(&t_ans);
    Fp_clear(&T0);
    Fp_clear(&T1);
    Fp_clear(&t0);
    Fp_clear(&t1);
    Fp_clear(&t2);
    Fp_clear(&t3);
}
void Fp3_div(struct Fp3 *ANS,struct Fp3 *A,struct Fp3 *B){
    struct Fp3 tmp,t_ans;
    Fp3_init(&tmp);
    Fp3_init(&t_ans);
    
    Fp3_invert(&tmp,B);
    Fp3_mul(&t_ans,A,&tmp);
    
    Fp3_set(ANS,&t_ans);
    
    Fp3_clear(&tmp);
    Fp3_clear(&t_ans);
}
void Fp3_pow(struct Fp3 *ANS,struct Fp3 *A,mpz_t B){
    int i;
    int r;//bit数
    r= (int)mpz_sizeinbase(B,2);
    //printf("r= %d\n",r);
    
    struct Fp3 answer_tmp;
    Fp3_init(&answer_tmp);
    Fp3_set(&answer_tmp,A);
    
    struct Fp3 in_tmp;
    Fp3_init(&in_tmp);
    Fp3_set(&in_tmp,A);
    
    for(i=r-2;i>=0;i--){
        if(mpz_tstbit(B,i)==1){
            Fp3_mul(&answer_tmp,&answer_tmp,&answer_tmp);//a*2
            Fp3_mul(&answer_tmp,&answer_tmp,&in_tmp);//*a
        }else{
            Fp3_mul(&answer_tmp,&answer_tmp,&answer_tmp);//a*2
        }
    }
    
    Fp3_set(ANS,&answer_tmp);
    
    Fp3_clear(&answer_tmp);
    Fp3_clear(&in_tmp);
}
void Fp3_sqrt(struct Fp3 *ANS,struct Fp3 *A){
    struct Fp3 n,y,x,b,t,tmp_Fp3;
    Fp3_init(&n);
    Fp3_init(&y);
    Fp3_init(&x);
    Fp3_init(&b);
    Fp3_init(&t);
    Fp3_init(&tmp_Fp3);
    Fp3_set(&n,A);
    
    mpz_t tmp_mpz,q,e,r,set_1,set_2;
    mpz_init(tmp_mpz);
    mpz_init(q);
    mpz_init(e);
    mpz_init(r);
    mpz_init(set_1);
    mpz_init(set_2);
    mpz_set_ui(set_1,1);
    mpz_set_ui(set_2,2);
    
    while(Fp3_legendre(&n)!=-1){
        Fp3_random(&n);
    }
    
    mpz_pow_ui(q,prime,3);
    mpz_sub_ui(q,q,1);
    mpz_set_ui(e,0);
    while(mpz_odd_p(q)==0){
        mpz_add_ui(e,e,1);
        mpz_div_ui(q,q,2);
    }
    Fp3_pow(&y,&n,q);
    mpz_set(r,e);
    mpz_sub_ui(tmp_mpz,q,1);
    mpz_div_ui(tmp_mpz,tmp_mpz,2);
    Fp3_pow(&x,A,tmp_mpz);
    Fp3_pow(&tmp_Fp3,&x,set_2);
    Fp3_mul(&b,&tmp_Fp3,A);
    Fp3_mul(&x,&x,A);
    int m;
    while(Fp3_cmp_mpz(&b,set_1)==1){
        m=-1;
        Fp3_set(&tmp_Fp3,&b);
        while(Fp3_cmp_mpz(&tmp_Fp3,set_1)==1){
            m++;
            mpz_pow_ui(tmp_mpz,set_2,m);
            Fp3_pow(&tmp_Fp3,&b,tmp_mpz);
        }
        mpz_sub_ui(tmp_mpz,r,m);
        mpz_sub_ui(tmp_mpz,tmp_mpz,1);
        mpz_powm(tmp_mpz,set_2,tmp_mpz,prime);
        Fp3_pow(&t,&y,tmp_mpz);
        Fp3_pow(&y,&t,set_2);
        mpz_set_ui(r,m);
        Fp3_mul(&x,&x,&t);
        Fp3_mul(&b,&b,&y);
    }
    
    Fp3_set(ANS,&x);
    
    Fp3_clear(&n);
    Fp3_clear(&y);
    Fp3_clear(&x);
    Fp3_clear(&b);
    Fp3_clear(&t);
    Fp3_clear(&tmp_Fp3);
    mpz_clear(tmp_mpz);
    mpz_clear(q);
    mpz_clear(e);
    mpz_clear(r);
    mpz_clear(set_1);
}
int Fp3_cmp(struct Fp3 *A,struct Fp3 *B){
    if(Fp_cmp(&A->x0,&B->x0)==0 && Fp_cmp(&A->x1,&B->x1)==0 && Fp_cmp(&A->x2,&B->x2)==0){
        return 0;
    }
    return 1;
}
int Fp3_cmp_mpz(struct Fp3 *A,mpz_t B){
    struct Fp3 tmp;
    Fp3_init(&tmp);
    if(Fp_cmp_mpz(&A->x0,B)==0 && Fp_cmp(&A->x1,&tmp.x1)==0 && Fp_cmp(&A->x2,&tmp.x2)==0){
        Fp3_clear(&tmp);
        return 0;
    }
    Fp3_clear(&tmp);
    return 1;
}
int Fp3_legendre(struct Fp3 *a){
    mpz_t i,cmp;
    mpz_init(i);
    mpz_init(cmp);
    mpz_set_ui(cmp,1);
    struct Fp3 tmp;
    Fp3_init(&tmp);
    mpz_pow_ui(i,prime,3);
    mpz_sub_ui(i,i,1);
    mpz_tdiv_q_ui(i,i,2);
    Fp3_pow(&tmp,a,i);
    
    if((Fp3_cmp_mpz(&tmp,cmp))==0){
        Fp3_clear(&tmp);
        mpz_clear(i);
        return 1;
    }else{
        Fp3_clear(&tmp);
        mpz_clear(i);
        return -1;
    }
}
void Fp3_neg(struct Fp3 *ans,struct Fp3 *a){
    struct Fp3 tmp;
    Fp3_init(&tmp);
    Fp_neg(&tmp.x0,&a->x0);
    Fp_neg(&tmp.x1,&a->x1);
    Fp_neg(&tmp.x2,&a->x2);
    Fp3_set(ans,&tmp);
    Fp3_clear(&tmp);
}

//-----------------------------------------------------------------------------------------
#pragma mark Fp6 methods
void Fp6_init(struct Fp6 *A){
    Fp3_init(&A->x0);
    Fp3_init(&A->x1);
}
void Fp6_set(struct Fp6 *ANS,struct Fp6 *A){
    Fp3_set(&ANS->x0,&A->x0);
    Fp3_set(&ANS->x1,&A->x1);
}
void Fp6_set_ui(struct Fp6 *A,signed long int B){
    Fp3_set_ui(&A->x0,B);
    Fp3_set_ui(&A->x1,B);
}
void Fp6_random(struct Fp6 *A){
    Fp3_random(&A->x0);
    Fp3_random(&A->x1);
}
void Fp6_clear(struct Fp6 *A){
    Fp3_clear(&A->x0);
    Fp3_clear(&A->x1);
}
void Fp6_printf(struct Fp6 *A){
    gmp_printf("%Zd,%Zd,%Zd,%Zd,%Zd,%Zd\n",A->x0.x0.x0,A->x0.x1.x0,A->x0.x2.x0,A->x1.x0.x0,A->x1.x1.x0,A->x1.x2.x0);
}
void Fp6_add(struct Fp6 *ANS,struct Fp6 *A,struct Fp6 *B){
    struct Fp6 tmp;
    Fp6_init(&tmp);
    
    Fp3_add(&tmp.x0,&A->x0,&B->x0);
    Fp3_add(&tmp.x1,&A->x1,&B->x1);
    
    Fp6_set(ANS,&tmp);
    
    Fp6_clear(&tmp);
}
void Fp6_add_ui(struct Fp6 *ANS,struct Fp6 *A,unsigned long int B){
    struct Fp6 tmp;
    Fp6_init(&tmp);
    
    Fp3_add_ui(&tmp.x0,&A->x0,B);
    Fp3_add_ui(&tmp.x1,&A->x1,B);
    
    Fp6_set(ANS,&tmp);
    
    Fp6_clear(&tmp);
}
void Fp6_sub(struct Fp6 *ANS,struct Fp6 *A,struct Fp6 *B){
    struct Fp6 tmp;
    Fp6_init(&tmp);
    
    Fp3_sub(&tmp.x0,&A->x0,&B->x0);
    Fp3_sub(&tmp.x1,&A->x1,&B->x1);
    
    Fp6_set(ANS,&tmp);
    
    Fp6_clear(&tmp);
}
void Fp6_mul(struct Fp6 *ANS,struct Fp6 *A,struct Fp6 *B){
    //x^2-v=0
    struct Fp3 tmp1,tmp2,tmp3,tmp4,tmp5,tmp6;
    Fp3_init(&tmp1);
    Fp3_init(&tmp2);
    Fp3_init(&tmp3);
    Fp3_init(&tmp4);
    Fp3_init(&tmp5);
    Fp3_init(&tmp6);
    
    struct Fp6 t_ans;
    Fp6_init(&t_ans);
    
    Fp3_mul(&tmp1,&A->x0,&B->x0);//a*c
    Fp3_mul(&tmp2,&A->x1,&B->x1);//b*d
    Fp3_mul_xi(&tmp3,&tmp2);//b*d*v
    Fp3_add(&t_ans.x0,&tmp1,&tmp3);//a*c+b*d*v
    Fp3_add(&tmp4,&A->x0,&A->x1);//a+b
    Fp3_add(&tmp5,&B->x0,&B->x1);//c+d
    Fp3_mul(&tmp6,&tmp4,&tmp5);//(a+b)(c+d)
    Fp3_sub(&t_ans.x1,&tmp6,&tmp1);
    Fp3_sub(&t_ans.x1,&t_ans.x1,&tmp2);
    
    Fp6_set(ANS,&t_ans);
    
    Fp3_clear(&tmp1);
    Fp3_clear(&tmp2);
    Fp3_clear(&tmp3);
    Fp3_clear(&tmp4);
    Fp3_clear(&tmp5);
    Fp3_clear(&tmp6);
    Fp6_clear(&t_ans);
}
void Fp6_mul_v(struct Fp6 *ANS,struct Fp6 *A){
    struct Fp6 tmp;
    Fp6_init(&tmp);
    
    Fp3_mul_xi(&tmp.x0,&A->x1);
    Fp3_set(&tmp.x1,&A->x0);
    
    Fp6_set(ANS,&tmp);
    Fp6_clear(&tmp);
}
void Fp6_mul_ui(struct Fp6 *ANS,struct Fp6 *A,unsigned long int B){
    struct Fp6 tmp;
    Fp6_init(&tmp);
    
    Fp3_mul_ui(&tmp.x0,&A->x0,B);
    Fp3_mul_ui(&tmp.x1,&A->x1,B);
    
    Fp6_set(ANS,&tmp);
    
    Fp6_clear(&tmp);
}
void Fp6_mul_Fp(struct Fp6 *ANS,struct Fp6 *A,struct Fp *B){
    struct Fp6 tmp;
    Fp6_init(&tmp);
    
    Fp3_mul_Fp(&tmp.x0,&A->x0,B);
    Fp3_mul_Fp(&tmp.x1,&A->x1,B);
    
    Fp6_set(ANS,&tmp);
    
    Fp6_clear(&tmp);
}
void Fp6_invert(struct Fp6 *ANS, struct Fp6 *A){
    struct Fp6 tmp;
    Fp6_init(&tmp);
    
    // tmp=A^(q^6)=(x0,-x1)
    Fp3_set(&tmp.x0,&A->x0);
    Fp3_neg(&tmp.x1,&A->x1);
    
    struct Fp3 c,a,b;
    Fp3_init(&c);
    Fp3_init(&a);
    Fp3_init(&b);
    
    Fp3_mul(&a,&A->x0,&A->x0); // a=x0^2
    Fp3_mul(&b,&A->x1,&A->x1); // b=x1^2
    Fp3_mul_xi(&b,&b); // b=x1^2*v
    Fp3_sub(&c,&a,&b); // c=x0^2-x1^2*v mod q
    
    Fp3_invert(&c,&c);
    
    // ANS=A^{-1}=(c)^{-1}*A^(p^6) A which c is Fp6-element and tmp is a vector A Fp6
    Fp3_mul(&tmp.x0,&tmp.x0,&c);
    Fp3_mul(&tmp.x1,&tmp.x1,&c);
    
    Fp6_set(ANS,&tmp);
    
    Fp3_clear(&c);
    Fp3_clear(&a);
    Fp3_clear(&b);
    Fp6_clear(&tmp);
}
void Fp6_div(struct Fp6 *ANS,struct Fp6 *A,struct Fp6 *B){
    struct Fp6 tmp,t_ans;
    Fp6_init(&tmp);
    Fp6_init(&t_ans);
    
    Fp6_invert(&tmp,B);
    Fp6_mul(&t_ans,A,&tmp);
    
    Fp6_set(ANS,&t_ans);
    
    Fp6_clear(&tmp);
    Fp6_clear(&t_ans);
}
void Fp6_pow(struct Fp6 *ANS,struct Fp6 *A,mpz_t B){
    int i;
    int r;//bit数
    r= (int)mpz_sizeinbase(B,2);
    //printf("r= %d\n",r);
    
    struct Fp6 answer_tmp;
    Fp6_init(&answer_tmp);
    Fp6_set(&answer_tmp,A);
    
    struct Fp6 in_tmp;
    Fp6_init(&in_tmp);
    Fp6_set(&in_tmp,A);
    
    for(i=r-2;i>=0;i--){
        if(mpz_tstbit(B,i)==1){
            Fp6_mul(&answer_tmp,&answer_tmp,&answer_tmp);//a*2
            Fp6_mul(&answer_tmp,&answer_tmp,&in_tmp);//*a
        }else{
            Fp6_mul(&answer_tmp,&answer_tmp,&answer_tmp);//a*2
        }
    }
    
    Fp6_set(ANS,&answer_tmp);
    
    Fp6_clear(&answer_tmp);
    Fp6_clear(&in_tmp);
}
void Fp6_sqrt(struct Fp6 *ANS,struct Fp6 *A){
    struct Fp6 n,y,x,b,t,tmp_Fp6;
    Fp6_init(&n);
    Fp6_init(&y);
    Fp6_init(&x);
    Fp6_init(&b);
    Fp6_init(&t);
    Fp6_init(&tmp_Fp6);
    Fp6_set(&n,A);
    
    mpz_t tmp_mpz,q,e,r,set_1,set_2;
    mpz_init(tmp_mpz);
    mpz_init(q);
    mpz_init(e);
    mpz_init(r);
    mpz_init(set_1);
    mpz_init(set_2);
    mpz_set_ui(set_1,1);
    mpz_set_ui(set_2,2);
    
    while(Fp6_legendre(&n)!=-1){
        Fp6_random(&n);
    }
    mpz_pow_ui(q,prime,6);
    mpz_sub_ui(q,q,1);
    mpz_set_ui(e,0);
    while(mpz_odd_p(q)==0){
        mpz_add_ui(e,e,1);
        mpz_div_ui(q,q,2);
    }
    Fp6_pow(&y,&n,q);
    
    mpz_set(r,e);
    
    mpz_sub_ui(tmp_mpz,q,1);
    mpz_div_ui(tmp_mpz,tmp_mpz,2);
    
    Fp6_pow(&x,A,tmp_mpz);
    Fp6_pow(&tmp_Fp6,&x,set_2);
    Fp6_mul(&b,&tmp_Fp6,A);
    Fp6_mul(&x,&x,A);
    
    int m;
    
    while(Fp6_cmp_mpz(&b,set_1)==1){
        m=-1;
        Fp6_set(&tmp_Fp6,&b);
        while(Fp6_cmp_mpz(&tmp_Fp6,set_1)==1){
            m++;
            mpz_pow_ui(tmp_mpz,set_2,m);
            Fp6_pow(&tmp_Fp6,&b,tmp_mpz);
        }
        mpz_sub_ui(tmp_mpz,r,m);
        mpz_sub_ui(tmp_mpz,tmp_mpz,1);
        mpz_powm(tmp_mpz,set_2,tmp_mpz,prime);
        // gmp_printf("%Zd,%Zd,%d\n",tmp_mpz,r,m);
        Fp6_pow(&t,&y,tmp_mpz);
        Fp6_pow(&y,&t,set_2);
        // gmp_printf("%Zd,%Zd,\n",y.x0.x0.x0,y.x0.x1.x0);
        mpz_set_ui(r,m);
        Fp6_mul(&x,&x,&t);
        Fp6_mul(&b,&b,&y);
    }
    
    Fp6_set(ANS,&x);
    
    Fp6_clear(&n);
    Fp6_clear(&y);
    Fp6_clear(&x);
    Fp6_clear(&b);
    Fp6_clear(&t);
    Fp6_clear(&tmp_Fp6);
    mpz_clear(tmp_mpz);
    mpz_clear(q);
    mpz_clear(e);
    mpz_clear(r);
    mpz_clear(set_1);
}
int Fp6_legendre(struct Fp6 *A){
    mpz_t i,cmp;
    struct Fp6 tmp;
    Fp6_init(&tmp);
    mpz_init(i);
    mpz_init(cmp);
    mpz_set_ui(cmp,1);
    mpz_pow_ui(i,prime,6);
    mpz_sub_ui(i,i,1);
    mpz_tdiv_q_ui(i,i,2);
    Fp6_pow(&tmp,A,i);
    
    if((Fp6_cmp_mpz(&tmp,cmp))==0){
        Fp6_clear(&tmp);
        mpz_clear(i);
        mpz_clear(cmp);
        return 1;
    }else{
        Fp6_clear(&tmp);
        mpz_clear(i);
        mpz_clear(cmp);
        return -1;
    }
}
int Fp6_cmp(struct Fp6 *A,struct Fp6 *B){
    if(Fp3_cmp(&A->x0,&B->x0)==0 && Fp3_cmp(&A->x1,&B->x1)==0){
        return 0;
    }
    return 1;
}
int Fp6_cmp_mpz(struct Fp6 *A,mpz_t B){
    struct Fp6 tmp;
    Fp6_init(&tmp);
    if(Fp3_cmp_mpz(&A->x0,B)==0 && Fp3_cmp(&A->x1,&tmp.x1)==0){
        Fp6_clear(&tmp);
        return 0;
    }
    Fp6_clear(&tmp);
    return 1;
}
void Fp6_neg(struct Fp6 *ANS,struct Fp6 *A){
    struct Fp6 tmp;
    Fp6_init(&tmp);
    Fp3_neg(&tmp.x0,&A->x0);
    Fp3_neg(&tmp.x1,&A->x1);
    Fp6_set(ANS,&tmp);
    Fp6_clear(&tmp);
}

//-----------------------------------------------------------------------------------------
#pragma mark Fp18 methods
void Fp18_init(struct Fp18 *A){
    Fp6_init(&A->x0);
    Fp6_init(&A->x1);
    Fp6_init(&A->x2);
}
void Fp18_set(struct Fp18 *ANS,struct Fp18 *A){
    Fp6_set(&ANS->x0,&A->x0);
    Fp6_set(&ANS->x1,&A->x1);
    Fp6_set(&ANS->x2,&A->x2);
}
void Fp18_set_ui(struct Fp18 *A,signed long int B){
    Fp6_set_ui(&A->x0,B);
    Fp6_set_ui(&A->x1,B);
    Fp6_set_ui(&A->x2,B);
}
void Fp18_random(struct Fp18 *A){
    Fp6_random(&A->x0);
    Fp6_random(&A->x1);
    Fp6_random(&A->x2);
}
void Fp18_clear(struct Fp18 *A){
    Fp6_clear(&A->x0);
    Fp6_clear(&A->x1);
    Fp6_clear(&A->x2);
}
void Fp18_printf(struct Fp18 *A){
    gmp_printf("%Zd,%Zd,%Zd,%Zd,%Zd,%Zd\n",A->x0.x0.x0.x0,A->x0.x0.x1.x0,A->x0.x0.x2.x0,A->x0.x1.x0.x0,A->x0.x1.x1.x0,A->x0.x1.x2.x0);
    gmp_printf("%Zd,%Zd,%Zd,%Zd,%Zd,%Zd\n",A->x1.x0.x0.x0,A->x1.x0.x1.x0,A->x1.x0.x2.x0,A->x1.x1.x0.x0,A->x1.x1.x1.x0,A->x1.x1.x2.x0);
    gmp_printf("%Zd,%Zd,%Zd,%Zd,%Zd,%Zd\n\n",A->x2.x0.x0.x0,A->x2.x0.x1.x0,A->x2.x0.x2.x0,A->x2.x1.x0.x0,A->x2.x1.x1.x0,A->x2.x1.x2.x0);
}
void Fp18_add(struct Fp18 *ANS,struct Fp18 *A,struct Fp18 *B){
    struct Fp18 tmp;
    Fp18_init(&tmp);
    
    Fp6_add(&tmp.x0,&A->x0,&B->x0);
    Fp6_add(&tmp.x1,&A->x1,&B->x1);
    Fp6_add(&tmp.x2,&A->x2,&B->x2);
    
    Fp18_set(ANS,&tmp);
    
    Fp18_clear(&tmp);
}
void Fp18_add_ui(struct Fp18 *ANS,struct Fp18 *A,unsigned long int B){
    struct Fp18 tmp;
    Fp18_init(&tmp);
    
    Fp6_add_ui(&tmp.x0,&A->x0,B);
    Fp6_add_ui(&tmp.x1,&A->x1,B);
    Fp6_add_ui(&tmp.x2,&A->x2,B);
    
    Fp18_set(ANS,&tmp);
    
    Fp18_clear(&tmp);
}
void Fp18_sub(struct Fp18 *ANS,struct Fp18 *A,struct Fp18 *B){
    struct Fp18 tmp;
    Fp18_init(&tmp);
    
    Fp6_sub(&tmp.x0,&A->x0,&B->x0);
    Fp6_sub(&tmp.x1,&A->x1,&B->x1);
    Fp6_sub(&tmp.x2,&A->x2,&B->x2);
    
    Fp18_set(ANS,&tmp);
    
    Fp18_clear(&tmp);
}
void Fp18_mul(struct Fp18 *ANS,struct Fp18 *A,struct Fp18 *B){
    //(x0,x1,x2)*(y0,y1,y2)=(x0y0+xi((x1+x2)(y1+y2)-x1y1-x2y2),xix2y2+(x0+x1)(y0+y1)-x0y0-x1y1,x1y1+(x0+x2)(y0+y2)-x0y0-x2y2)
    struct Fp6 tmp00,tmp11,tmp22,tmpx01,tmpx12,tmpx20,tmpy01,tmpy12,tmpy20,t0,t1,t2,tmp;
    struct Fp18 t_ans;
    Fp6_init(&tmp00);
    Fp6_init(&tmp11);
    Fp6_init(&tmp22);
    Fp6_init(&tmpx01);
    Fp6_init(&tmpx12);
    Fp6_init(&tmpx20);
    Fp6_init(&tmpy01);
    Fp6_init(&tmpy12);
    Fp6_init(&tmpy20);
    Fp6_init(&t0);
    Fp6_init(&t1);
    Fp6_init(&t2);
    Fp6_init(&tmp);
    Fp18_init(&t_ans);
    
    Fp6_mul(&tmp00,&A->x0,&B->x0);//x0*y0
    Fp6_mul(&tmp11,&A->x1,&B->x1);//x1*y1
    Fp6_mul(&tmp22,&A->x2,&B->x2);//x2*y2
    
    Fp6_add(&tmpx01,&A->x0,&A->x1);//x0+x1
    Fp6_add(&tmpx12,&A->x1,&A->x2);//x1+x2
    Fp6_add(&tmpx20,&A->x0,&A->x2);//x2+x0
    Fp6_add(&tmpy01,&B->x0,&B->x1);//y0+y1
    Fp6_add(&tmpy12,&B->x1,&B->x2);//y1+y2
    Fp6_add(&tmpy20,&B->x0,&B->x2);//y2+y0
    
    Fp6_mul(&t0,&tmpx01,&tmpy01);//(x0+x1)(y0+y1)
    Fp6_mul(&t1,&tmpx12,&tmpy12);//(x1+x2)(y1+y2)
    Fp6_mul(&t2,&tmpx20,&tmpy20);//(x2+x0)(y2+y0)
    
    Fp6_sub(&t1,&t1,&tmp11);
    Fp6_sub(&t1,&t1,&tmp22);//(x1+x2)(y1+y2)-x1y1-x2y2
    Fp6_mul_v(&tmp,&t1);
    Fp6_add(&t_ans.x0,&tmp00,&tmp);
    
    Fp6_sub(&t0,&t0,&tmp00);
    Fp6_sub(&t0,&t0,&tmp11);
    Fp6_mul_v(&tmp,&tmp22);
    Fp6_add(&t_ans.x1,&tmp,&t0);
    
    Fp6_sub(&t2,&t2,&tmp00);
    Fp6_sub(&t2,&t2,&tmp22);
    Fp6_add(&t_ans.x2,&tmp11,&t2);
    
    Fp18_set(ANS,&t_ans);
    
    Fp6_clear(&tmp00);
    Fp6_clear(&tmp11);
    Fp6_clear(&tmp22);
    Fp6_clear(&tmpx01);
    Fp6_clear(&tmpx12);
    Fp6_clear(&tmpx20);
    Fp6_clear(&tmpy01);
    Fp6_clear(&tmpy12);
    Fp6_clear(&tmpy20);
    Fp6_clear(&t0);
    Fp6_clear(&t1);
    Fp6_clear(&t2);
    Fp6_clear(&tmp);
}
void Fp18_mul_ui(struct Fp18 *ANS,struct Fp18 *A,unsigned long int B){
    struct Fp18 tmp;
    Fp18_init(&tmp);
    
    Fp6_mul_ui(&tmp.x0,&A->x0,B);
    Fp6_mul_ui(&tmp.x1,&A->x1,B);
    Fp6_mul_ui(&tmp.x2,&A->x2,B);
    
    Fp18_set(ANS,&tmp);
    Fp18_clear(&tmp);
}
void Fp18_mul_Fp(struct Fp18 *ANS,struct Fp18 *A,struct Fp *B){
    struct Fp18 tmp;
    Fp18_init(&tmp);
    
    Fp6_mul_Fp(&tmp.x0,&A->x0,B);
    Fp6_mul_Fp(&tmp.x1,&A->x1,B);
    Fp6_mul_Fp(&tmp.x2,&A->x2,B);
    
    Fp18_set(ANS,&tmp);
    
    Fp18_clear(&tmp);
}
void Fp18_invert(struct Fp18 *ANS, struct Fp18 *A){
    struct Fp18 t_ans;
    Fp18_init(&t_ans);
    
    struct Fp6 T0,T1,t0,t1,t2,t3;
    Fp6_init(&T0);
    Fp6_init(&T1);
    Fp6_init(&t0);
    Fp6_init(&t1);
    Fp6_init(&t2);
    Fp6_init(&t3);
    
    // An optimized version of Grewal's Algo. 3   (a,b,c)
    Fp6_mul(&T0,&A->x0,&A->x0);
    Fp6_mul_v(&t0,&A->x1);
    
    Fp6_mul(&T1,&t0,&A->x2);
    Fp6_sub(&t1,&T0,&T1); // t1=(a^2-bci) mod q
    
    Fp6_mul(&T0,&A->x2,&A->x2);
    Fp6_mul_v(&T0,&T0);
    Fp6_mul(&T1,&A->x0,&A->x1);
    Fp6_sub(&t2,&T0,&T1); // t2=(c^2i-ab) mod q
    
    Fp6_mul(&T0,&A->x1,&A->x1);
    Fp6_mul(&T1,&A->x0,&A->x2);
    Fp6_sub(&t3,&T0,&T1); // t3=(b^2-ac) mod q
    
    Fp6_mul(&T0,&t0,&t3);
    Fp6_mul(&T1,&A->x0,&t1);
    Fp6_add(&T0,&T0,&T1); // T0={bi(b^2-ac)+a(a^2-bci)} mod q
    
    Fp6_mul_v(&t0,&A->x2);
    Fp6_mul(&T1,&t0,&t2);
    Fp6_add(&t0,&T0,&T1); // t0={ci(c^2i-ab)+{bi(b^2-ac)+a(a^2-bci)}} mod q .0
    
    Fp6_invert(&t0,&t0);
    
    Fp6_mul(&t_ans.x0,&t1,&t0);
    Fp6_mul(&t_ans.x1,&t2,&t0);
    Fp6_mul(&t_ans.x2,&t3,&t0);
    
    Fp18_set(ANS,&t_ans);
    
    Fp18_clear(&t_ans);
    Fp6_clear(&T0);
    Fp6_clear(&T1);
    Fp6_clear(&t0);
    Fp6_clear(&t1);
    Fp6_clear(&t2);
    Fp6_clear(&t3);
}
void Fp18_div(struct Fp18 *ANS,struct Fp18 *A,struct Fp18 *B){
    struct Fp18 tmp,t_ans;
    Fp18_init(&tmp);
    Fp18_init(&t_ans);
    
    Fp18_invert(&tmp,B);
    Fp18_mul(&t_ans,A,&tmp);
    
    Fp18_set(ANS,&t_ans);
    
    Fp18_clear(&tmp);
    Fp18_clear(&t_ans);
}
void Fp18_pow(struct Fp18 *ANS,struct Fp18 *A,mpz_t B){
    int i;
    int r;//bit数
    r= (int)mpz_sizeinbase(B,2);
    //printf("r= %d\n",r);
    
    struct Fp18 answer_tmp;
    Fp18_init(&answer_tmp);
    Fp18_set(&answer_tmp,A);
    
    struct Fp18 in_tmp;
    Fp18_init(&in_tmp);
    Fp18_set(&in_tmp,A);
    
    for(i=r-2;i>=0;i--){
        if(mpz_tstbit(B,i)==1){
            Fp18_mul(&answer_tmp,&answer_tmp,&answer_tmp);//a*2
            Fp18_mul(&answer_tmp,&answer_tmp,&in_tmp);//*a
        }else{
            Fp18_mul(&answer_tmp,&answer_tmp,&answer_tmp);//a*2
        }
    }
    
    Fp18_set(ANS,&answer_tmp);
    
    Fp18_clear(&answer_tmp);
    Fp18_clear(&in_tmp);
}
void Fp18_sqrt(struct Fp18 *ANS,struct Fp18 *A){
    struct Fp18 n,y,x,b,t,tmp_Fp18;
    Fp18_init(&n);
    Fp18_init(&y);
    Fp18_init(&x);
    Fp18_init(&b);
    Fp18_init(&t);
    Fp18_init(&tmp_Fp18);
    Fp18_set(&n,A);
    
    mpz_t tmp_mpz,q,e,r,set_1,set_2;
    mpz_init(tmp_mpz);
    mpz_init(q);
    mpz_init(e);
    mpz_init(r);
    mpz_init(set_1);
    mpz_init(set_2);
    mpz_set_ui(set_1,1);
    mpz_set_ui(set_2,2);
    
    while(Fp18_legendre(&n)!=-1){
        Fp18_random(&n);
        // printf("%d\n",Fp18_legendre(&n));
    }
    
    mpz_pow_ui(q,prime,18);
    mpz_sub_ui(q,q,1);
    mpz_set_ui(e,0);
    while(mpz_odd_p(q)==0){
        
        mpz_add_ui(e,e,1);
        mpz_div_ui(q,q,2);
    }
    Fp18_pow(&y,&n,q);
    mpz_set(r,e);
    mpz_sub_ui(tmp_mpz,q,1);
    mpz_div_ui(tmp_mpz,tmp_mpz,2);
    Fp18_pow(&x,A,tmp_mpz);
    Fp18_pow(&tmp_Fp18,&x,set_2);
    Fp18_mul(&b,&tmp_Fp18,A);
    Fp18_mul(&x,&x,A);
    int m;
    while(Fp18_cmp_mpz(&b,set_1)==1){
        m=-1;
        Fp18_set(&tmp_Fp18,&b);
        while(Fp18_cmp_mpz(&tmp_Fp18,set_1)==1){
            m++;
            mpz_pow_ui(tmp_mpz,set_2,m);
            Fp18_pow(&tmp_Fp18,&b,tmp_mpz);
        }
        mpz_sub_ui(tmp_mpz,r,m);
        mpz_sub_ui(tmp_mpz,tmp_mpz,1);
        mpz_powm(tmp_mpz,set_2,tmp_mpz,prime);
        Fp18_pow(&t,&y,tmp_mpz);
        Fp18_pow(&y,&t,set_2);
        mpz_set_ui(r,m);
        Fp18_mul(&x,&x,&t);
        Fp18_mul(&b,&b,&y);
    }
    
    Fp18_set(ANS,&x);
    
    Fp18_clear(&n);
    Fp18_clear(&y);
    Fp18_clear(&x);
    Fp18_clear(&b);
    Fp18_clear(&t);
    Fp18_clear(&tmp_Fp18);
    mpz_clear(tmp_mpz);
    mpz_clear(q);
    mpz_clear(e);
    mpz_clear(r);
    mpz_clear(set_1);
}
int Fp18_cmp(struct Fp18 *A,struct Fp18 *B){
    if(Fp6_cmp(&A->x0,&B->x0)==0 && Fp6_cmp(&A->x1,&B->x1)==0 && Fp6_cmp(&A->x2,&B->x2)==0){
        return 0;
    }
    return 1;
}
int Fp18_cmp_mpz(struct Fp18 *A,mpz_t B){
    struct Fp18 tmp;
    Fp18_init(&tmp);
    if(Fp6_cmp_mpz(&A->x0,B)==0 && Fp6_cmp(&A->x1,&tmp.x1)==0 && Fp6_cmp(&A->x2,&tmp.x2)==0){
        Fp18_clear(&tmp);
        return 0;
    }
    Fp18_clear(&tmp);
    return 1;
}
int Fp18_legendre(struct Fp18 *A){
    mpz_t i,cmp;
    struct Fp18 tmp;
    mpz_init(i);
    mpz_init(cmp);
    Fp18_init(&tmp);
    mpz_set_ui(cmp,1);
    mpz_pow_ui(i,prime,18);
    mpz_sub_ui(i,i,1);
    mpz_tdiv_q_ui(i,i,2);
    Fp18_pow(&tmp,A,i);
    
    if((Fp18_cmp_mpz(&tmp,cmp))==0){
        Fp18_clear(&tmp);
        mpz_clear(i);
        mpz_clear(cmp);
        return 1;
    }else{
        Fp18_clear(&tmp);
        mpz_clear(i);
        mpz_clear(cmp);
        return -1;
    }
}
void Fp18_neg(struct Fp18 *ANS,struct Fp18 *A){
    struct Fp18 tmp;
    Fp18_init(&tmp);
    Fp6_neg(&tmp.x0,&A->x0);
    Fp6_neg(&tmp.x1,&A->x1);
    Fp6_neg(&tmp.x2,&A->x2);
    Fp18_set(ANS,&tmp);
    Fp18_clear(&tmp);
}
void Fp18_frobenius_map(struct Fp18 *ANS, struct Fp18 *A){
    struct Fp18 tmp;
    Fp18_init(&tmp);
    
    Fp18_pow(&tmp,A,prime);
    Fp18_set(ANS,&tmp);
    Fp18_clear(&tmp);
}
//-----------------------------------------------------------------------------------------
#pragma mark EFp methods
void EFp_init(struct EFp *A){
    Fp_init(&A->x);
    Fp_init(&A->y);
    A->infity=FALSE;
}
void EFp_set(struct EFp *A,struct EFp *B){
    Fp_set(&A->x,&B->x);
    Fp_set(&A->y,&B->y);
    A->infity=B->infity;
}
void EFp_set_infity(struct EFp *A){
    Fp_set_ui(&A->x,0);
    Fp_set_ui(&A->y,0);
    A->infity=TRUE;
}
void EFp_clear(struct EFp *A){
    Fp_clear(&A->x);
    Fp_clear(&A->y);
}
void EFp_printf(struct EFp *A){
    gmp_printf("(%Zd,%Zd)\n",A->x.x0,A->y.x0);
}
void EFp_SCM_BIN(struct EFp *ANS, struct EFp *P,mpz_t j){
    int i;
    int r;//bit数
    r= (int)mpz_sizeinbase(j,2);
    
    struct EFp Q;
    EFp_init(&Q);
    EFp_set(&Q,P);
    
    for(i=r-2;i>=0;i--){
        if(mpz_tstbit(j,i)==1){
            EFp_ECD(&Q,&Q);
            EFp_ECA(&Q,&Q,P);
        }else{
            EFp_ECD(&Q,&Q);
        }
    }
    
    EFp_set(ANS,&Q);
    EFp_clear(&Q);
    return;
}
//TODO: NAF and SLIDING WINDOW
void TO_NAF(mpz_t scalar)
{
    int size = (int)mpz_sizeinbase(scalar,2);
    myNAF[size];
    naf_index = 0;
    mpz_t zi,tmp1,tmp2,scalar_tmp;
    mpz_init(zi);
    mpz_init(tmp1);
    mpz_init(tmp2);
    mpz_init(scalar_tmp);
    mpz_set(scalar_tmp,scalar);
    //    char x1_binary[(int)mpz_sizeinbase(scalar_tmp,2)];
    //    mpz_get_str(x1_binary,2,scalar_tmp);
    //    printf("scalar binary %c\n",x1_binary[5]);
    
    while (mpz_cmp_ui(scalar_tmp, 0) > 0) {
        
        if (mpz_odd_p(scalar_tmp)){
            mpz_mod_ui(tmp1,scalar_tmp,4);
            mpz_set_ui(tmp2,2);
            mpz_sub(zi,tmp2,tmp1);
            mpz_sub(scalar_tmp,scalar_tmp,zi);
        }
        else{
            mpz_set_ui(zi,0);
        }
        mpz_tdiv_q_2exp(scalar_tmp,scalar_tmp,1);
        myNAF[naf_index] = mpz_get_si(zi);
        naf_index++;
    }
//    printf("To NAF index %ld\n",naf_index);
    signed long temp, end;
    end = naf_index - 1;
    for (int c = 0; c < naf_index/2; c++)
    {
        temp          = myNAF[c];
        myNAF[c]   = myNAF[end];
        myNAF[end] = temp;
        end--;
    }
}
//NAF
void EFp_SCM_NAF(struct EFp *ANS, struct EFp *P, mpz_t scalar)
{
    TO_NAF(scalar);
    struct EFp Q,R;
    EFp_init(&Q);
    EFp_set_infity(&Q);
    EFp_init(&R);
    
    for (int j = 0; j< naf_index; j++){
        EFp_ECD(&Q,&Q);
        if(myNAF[j]== 1){
            EFp_ECA(&Q,&Q,P);
        }
        else if(myNAF[j] == -1){
            EFp_neg(&R, P);
            EFp_ECA(&Q,&Q,&R);
        }
    }
    EFp_set(ANS,&Q);
    EFp_clear(&Q);
    EFp_clear(&R);
    return;
}

void EFp_SCM_WIN(struct EFp *ANS, struct EFp *P, mpz_t scalar){
    int i,length;
    length= (int)mpz_sizeinbase(scalar,2);
    char r_binary[length];
    mpz_get_str(r_binary,2,scalar);
    
    int win_size = 4;
    int rem = length % win_size;
    int mul3 = length - rem;
    
    
    struct EFp R,R2,R3,R4,R5,R6,R7,R8,R9,R10,R11,R12,R13,R14,R15,ANS_temp;
    EFp_init(&R);
    EFp_init(&R2);
    EFp_init(&R3);
    EFp_init(&R4);
    EFp_init(&R5);
    EFp_init(&R6);
    EFp_init(&R7);
    EFp_init(&R8);
    EFp_init(&R9);
    EFp_init(&R10);
    EFp_init(&R11);
    EFp_init(&R12);
    EFp_init(&R13);
    EFp_init(&R14);
    EFp_init(&R15);
    EFp_init(&ANS_temp);
    
    EFp_set_infity(&ANS_temp);
    EFp_set(&R, P);
    EFp_ECA(&R2, &R, &R);
    EFp_ECA(&R3, &R2, &R);
    EFp_ECA(&R4, &R3, &R);
    EFp_ECA(&R5, &R4, &R);
    EFp_ECA(&R6, &R5, &R);
    EFp_ECA(&R7, &R6, &R);
    EFp_ECA(&R8, &R7, &R);
    EFp_ECA(&R9, &R8, &R);
    EFp_ECA(&R10, &R9, &R);
    EFp_ECA(&R11, &R10, &R);
    EFp_ECA(&R12, &R11, &R);
    EFp_ECA(&R13, &R12, &R);
    EFp_ECA(&R14, &R13, &R);
    EFp_ECA(&R15, &R14, &R);
    
    for(i=0; i< mul3;i=i+4){
        
        EFp_ECD(&ANS_temp,&ANS_temp);
        EFp_ECD(&ANS_temp,&ANS_temp);
        EFp_ECD(&ANS_temp,&ANS_temp);
        EFp_ECD(&ANS_temp,&ANS_temp);
        
        if (i+2 < length)
        {
            if(r_binary[i] =='1' && r_binary[i+1] =='1' && r_binary[i+2] =='1' && r_binary[i+3] =='1'){
                EFp_ECA(&ANS_temp, &ANS_temp, &R15);
            }
            else if(r_binary[i] =='1' && r_binary[i+1] =='1' && r_binary[i+2] =='1' && r_binary[i+3] =='0'){
                EFp_ECA(&ANS_temp, &ANS_temp, &R14);
            }
            else if(r_binary[i] =='1' && r_binary[i+1] =='1' && r_binary[i+2] =='0' && r_binary[i+3] =='1'){
                EFp_ECA(&ANS_temp, &ANS_temp, &R13);
            }
            else if(r_binary[i] =='1' && r_binary[i+1] =='1' && r_binary[i+2] =='0' && r_binary[i+3] =='0'){
                EFp_ECA(&ANS_temp, &ANS_temp, &R12);
            }
            else if(r_binary[i] =='1' && r_binary[i+1] =='0' && r_binary[i+2] =='1' && r_binary[i+3] =='1'){
                EFp_ECA(&ANS_temp, &ANS_temp, &R11);
            }
            else if(r_binary[i] =='1' && r_binary[i+1] =='0' && r_binary[i+2] =='1' && r_binary[i+3] =='0'){
                EFp_ECA(&ANS_temp, &ANS_temp, &R10);
            }
            else if(r_binary[i] =='1' && r_binary[i+1] =='0' && r_binary[i+2] =='0' && r_binary[i+3] =='1'){
                EFp_ECA(&ANS_temp, &ANS_temp, &R9);
            }
            else if(r_binary[i] =='1' && r_binary[i+1] =='0' && r_binary[i+2] =='0' && r_binary[i+3] =='0'){
                EFp_ECA(&ANS_temp, &ANS_temp, &R8);
            }
            else if(r_binary[i] =='0' && r_binary[i+1] =='1' && r_binary[i+2] =='1' && r_binary[i+3] =='1'){
                EFp_ECA(&ANS_temp, &ANS_temp, &R7);
            }
            else if(r_binary[i] =='0' && r_binary[i+1] =='1' && r_binary[i+2] =='1' && r_binary[i+3] =='0'){
                EFp_ECA(&ANS_temp, &ANS_temp, &R6);
            }
            else if(r_binary[i] =='0' && r_binary[i+1] =='1' && r_binary[i+2] =='0' && r_binary[i+3] =='1'){
                EFp_ECA(&ANS_temp, &ANS_temp, &R5);
            }
            else if(r_binary[i] =='0' && r_binary[i+1] =='1' && r_binary[i+2] =='0' && r_binary[i+3] =='0'){
                EFp_ECA(&ANS_temp, &ANS_temp, &R4);
            }
            else if(r_binary[i] =='0' && r_binary[i+1] =='0' && r_binary[i+2] =='1' && r_binary[i+3] =='1'){
                EFp_ECA(&ANS_temp, &ANS_temp, &R3);
            }
            else if(r_binary[i] =='0' && r_binary[i+1] =='0' && r_binary[i+2] =='1' && r_binary[i+3] =='0'){
                EFp_ECA(&ANS_temp, &ANS_temp, &R2);
            }
            else if(r_binary[i] =='0' && r_binary[i+1] =='0' && r_binary[i+2] =='0' && r_binary[i+3] =='1'){
                EFp_ECA(&ANS_temp, &ANS_temp, &R);
            }
            else{
            }
        }
    }
    
    for(i = mul3;r_binary[i]!='\0';i++){
        EFp_ECD(&ANS_temp,&ANS_temp);
        if(r_binary[i]=='1'){
            EFp_ECA(&ANS_temp,&ANS_temp,P);
        }
    }
    EFp_set(ANS,&ANS_temp);
    EFp_clear(&ANS_temp);
    EFp_clear(&R);
    EFp_clear(&R2);
    EFp_clear(&R3);
    EFp_clear(&R4);
    EFp_clear(&R5);
    EFp_clear(&R6);
    EFp_clear(&R7);
    return;
}
void EFp_ECD(struct EFp *ANS, struct EFp *P){
    if(P->infity==TRUE){
        EFp_set(ANS,P);
        return;
    }
    if(mpz_sgn(P->y.x0)==0){//P.y==0
        EFp_set_infity(ANS);
        return;
    }
    
    struct Fp x,y,lambda,tmp;
    struct EFp t_ans;
    Fp_init(&x);
    Fp_init(&lambda);
    Fp_init(&tmp);
    Fp_init(&y);
    EFp_init(&t_ans);
    
    Fp_mul(&x,&P->x,&P->x);
    Fp_add(&tmp,&x,&x);
    Fp_add(&x,&tmp,&x);//3x^2
    Fp_add(&y,&P->y,&P->y);//2y
    
    Fp_div(&lambda,&x,&y);
    Fp_mul(&tmp,&lambda,&lambda);
    Fp_add(&x,&P->x,&P->x);
    Fp_sub(&x,&tmp,&x);
    Fp_sub(&tmp,&P->x,&x);
    
    Fp_set(&t_ans.x,&x);
    
    Fp_mul(&tmp,&tmp,&lambda);
    Fp_sub(&t_ans.y,&tmp,&P->y);
    
    EFp_set(ANS,&t_ans);
    
    Fp_clear(&x);
    Fp_clear(&lambda);
    Fp_clear(&y);
    Fp_clear(&tmp);
    EFp_clear(&t_ans);
}
void EFp_ECA(struct EFp *ANS, struct EFp *P1, struct EFp *P2){
    if(P2->infity==TRUE){//if P2==inf
        EFp_set(ANS,P1);
        return;
    }
    else if(P1->infity==TRUE){//if P1==inf
        EFp_set(ANS,P2);
        return;
    }
    else if(Fp_cmp(&P1->x,&P2->x)==0&&Fp_cmp(&P1->y,&P2->y)==1){ //P1.x==P2.x&&P1.y!=P2.y
        EFp_set_infity(ANS);
        return;
    }
    else if(EFp_cmp(P1,P2)==0){ // P=Q
        EFp_ECD(ANS,P1);
        return;
    }
    
    struct Fp x,y,lambda,tmp;
    struct EFp t_ans;
    
    Fp_init(&x);
    Fp_init(&y);
    Fp_init(&lambda);
    Fp_init(&tmp);
    EFp_init(&t_ans);
    
    Fp_sub(&x,&P2->x,&P1->x);
    Fp_sub(&y,&P2->y,&P1->y);
    Fp_div(&lambda,&y,&x);
    Fp_mul(&tmp,&lambda,&lambda);
    Fp_add(&x,&P1->x,&P2->x);
    Fp_sub(&x,&tmp,&x);
    Fp_sub(&tmp,&P1->x,&x);
    Fp_set(&t_ans.x,&x);
    Fp_mul(&tmp,&tmp,&lambda);
    Fp_sub(&t_ans.y,&tmp,&P1->y);
    
    EFp_set(ANS,&t_ans);
    
    Fp_clear(&x);
    Fp_clear(&y);
    Fp_clear(&lambda);
    Fp_clear(&tmp);
    EFp_clear(&t_ans);
}
int EFp_cmp(struct EFp *A,struct EFp *B){
    if(Fp_cmp(&A->x,&B->x)==0 && Fp_cmp(&A->y,&B->y)==0){
        return 0;
    }
    return 1;
}
void EFp_neg(struct EFp *ANS, struct EFp *A){
    struct EFp tmp;
    EFp_init(&tmp);
    Fp_neg(&tmp.y,&A->y);
    Fp_set(&tmp.x,&A->x);
    EFp_set(ANS,&tmp);
    EFp_clear(&tmp);
}
void EFp_random_set(struct EFp *ANS){
    struct Fp a,x;
    Fp_init(&a);
    Fp_init(&x);
    
    struct EFp P,Q;
    EFp_init(&P);
    EFp_init(&Q);
    
    mpz_t r_div_2;
    mpz_init(r_div_2);
    mpz_div_ui(r_div_2,order_r,2);
    // gmp_printf("%Zd\n",r_div_2);
    do{
        do{
            Fp_random(&x);
            Fp_mul(&a,&x,&x);
            Fp_mul(&a,&a,&x);
            mpz_add(a.x0,a.x0,b);
        }while(mpz_legendre(a.x0,prime)!=1);
        Q.infity=0;
        Fp_sqrt(&P.y,&a);
        Fp_set(&P.x,&x);
        
        EFp_SCM_BIN(&Q,&P,r_div_2);
    }while(Q.infity==TRUE);
    EFp_set(ANS,&P);
    
    
    Fp_clear(&a);
    Fp_clear(&x);
    EFp_clear(&P);
}

//-----------------------------------------------------------------------------------------
#pragma mark EFp3 methods
void EFp3_init(struct EFp3 *A){
    Fp3_init(&A->x);
    Fp3_init(&A->y);
    A->infity=FALSE;
}
void EFp3_set(struct EFp3 *A,struct EFp3 *B){
    Fp3_set(&A->x,&B->x);
    Fp3_set(&A->y,&B->y);
    A->infity=B->infity;
}
void EFp3_set_infity(struct EFp3 *A){
    Fp3_set_ui(&A->x,0);
    Fp3_set_ui(&A->y,0);
    A->infity=TRUE;
}
void EFp3_clear(struct EFp3 *A){
    Fp3_clear(&A->x);
    Fp3_clear(&A->y);
}
void EFp3_printf(struct EFp3 *A){
    gmp_printf("(%Zd,%Zd,%Zd,%Zd,%Zd,%Zd)\n",A->x.x0.x0,A->x.x1.x0,A->x.x2.x0,A->y.x0.x0,A->y.x1.x0,A->y.x2.x0);
}
void EFp3_ECD(struct EFp3 *ANS, struct EFp3 *P){
    if(P->infity==TRUE){
        EFp3_set(ANS,P);
        return;
    }
    mpz_t cmp;
    mpz_init(cmp);
    mpz_set_ui(cmp,0);
    if(Fp3_cmp_mpz(&P->y,cmp)==0){//P.y==0
        EFp3_set_infity(ANS);
        return;
    }
    
    struct Fp3 x,y,lambda,tmp;
    struct EFp3 t_ans;
    Fp3_init(&x);
    Fp3_init(&lambda);
    Fp3_init(&tmp);
    Fp3_init(&y);
    EFp3_init(&t_ans);
    
    Fp3_mul(&x,&P->x,&P->x);
    Fp3_add(&tmp,&x,&x);
    Fp3_add(&x,&tmp,&x);
    Fp3_add(&y,&P->y,&P->y);
    Fp3_div(&lambda,&x,&y);
    Fp3_mul(&tmp,&lambda,&lambda);
    Fp3_add(&x,&P->x,&P->x);
    Fp3_sub(&x,&tmp,&x);
    Fp3_sub(&tmp,&P->x,&x);
    Fp3_set(&t_ans.x,&x);
    Fp3_mul(&tmp,&tmp,&lambda);
    Fp3_sub(&t_ans.y,&tmp,&P->y);
    
    EFp3_set(ANS,&t_ans);
    
    Fp3_clear(&x);
    Fp3_clear(&lambda);
    Fp3_clear(&y);
    Fp3_clear(&tmp);
    EFp3_clear(&t_ans);
}
void EFp3_ECA(struct EFp3 *ANS, struct EFp3 *P1, struct EFp3 *P2){
    if(P2->infity==TRUE){//if P2==inf
        EFp3_set(ANS,P1);
        return;
    }
    else if(P1->infity==TRUE){//if P1==inf
        EFp3_set(ANS,P2);
        return;
    }
    else if(Fp3_cmp(&P1->x,&P2->x)==0&&Fp3_cmp(&P1->y,&P2->y)==1){ //P1.x==P2.x&&P1.y!=P2.y
        EFp3_set_infity(ANS);
        return;
    }
    else if(EFp3_cmp(P1,P2)==0){ // P=Q
        EFp3_ECD(ANS,P1);
        return;
    }
    
    struct Fp3 x,y,lambda,tmp;
    struct EFp3 t_ans;
    
    Fp3_init(&x);
    Fp3_init(&y);
    Fp3_init(&lambda);
    Fp3_init(&tmp);
    EFp3_init(&t_ans);
    
    Fp3_sub(&x,&P2->x,&P1->x);
    Fp3_sub(&y,&P2->y,&P1->y);
    Fp3_div(&lambda,&y,&x);
    Fp3_mul(&tmp,&lambda,&lambda);
    Fp3_add(&x,&P1->x,&P2->x);
    Fp3_sub(&x,&tmp,&x);
    Fp3_sub(&tmp,&P1->x,&x);
    Fp3_set(&t_ans.x,&x);
    Fp3_mul(&tmp,&tmp,&lambda);
    Fp3_sub(&t_ans.y,&tmp,&P1->y);
    
    EFp3_set(ANS,&t_ans);
    
    Fp3_clear(&x);
    Fp3_clear(&y);
    Fp3_clear(&lambda);
    Fp3_clear(&tmp);
    EFp3_clear(&t_ans);
}
void EFp3_SCM_BIN(struct EFp3 *ANS, struct EFp3 *P,mpz_t j){
    int i;
    int r;//bit数
    r= (int)mpz_sizeinbase(j,2);
    
    struct EFp3 Q;
    EFp3_init(&Q);
    EFp3_set(&Q,P);
    count_eca_BIN_map = 0; count_ecd_BIN_map=0;
    for(i=r-2;i>=0;i--){
        if(mpz_tstbit(j,i)==1){
            EFp3_ECD(&Q,&Q);
            EFp3_ECA(&Q,&Q,P);
            count_eca_BIN_map++;
            count_ecd_BIN_map++;
        }else{
            EFp3_ECD(&Q,&Q);
            count_ecd_BIN_map++;
        }
    }
    
    EFp3_set(ANS,&Q);
    EFp3_clear(&Q);
    return;
}
//NAF
void EFp3_SCM_NAF(struct EFp3 *ANS, struct EFp3 *P, mpz_t scalar)
{
    count_eca_NAF_map = 0; count_ecd_NAF_map = 0;
    TO_NAF(scalar);
    struct EFp3 Q,R;
    EFp3_init(&Q);
    EFp3_set_infity(&Q);
    EFp3_init(&R);
    //    int arrLength = (int)( sizeof(myNAF) / sizeof(myNAF[0]));
    //    printf("Length of Array %d",arrLength);
    
    for (int j = 0; j< naf_index; j++){
        printf("J %ld\n",myNAF[j]);
        EFp3_ECD(&Q,&Q);
        count_ecd_NAF_map++;
        if(myNAF[j]== 1){
            EFp3_ECA(&Q,&Q,P);
            count_eca_NAF_map++;
        }
        else if(myNAF[j] == -1){
            EFp3_neg(&R, P);
            EFp3_ECA(&Q,&Q,&R);
            count_eca_NAF_map++;
        }
    }
    EFp3_set(ANS,&Q);
    EFp3_clear(&Q);
    EFp3_clear(&R);
    return;
}
void EFp3_SCM_WIN_3(struct EFp3 *ANS, struct EFp3 *P, mpz_t scalar){
    int i,length;
    length= (int)mpz_sizeinbase(scalar,2);
    char r_binary[length];
    mpz_get_str(r_binary,2,scalar);
    
    int rem = length % 3;
    int mul3 = length - rem;
    
    count_eca_WIN_map = 0; count_ecd_WIN_map =0;
    struct EFp3 R,R2,R3,R4,R5,R6,R7,ANS_temp;
    EFp3_init(&R);
    EFp3_init(&R2);
    EFp3_init(&R3);
    EFp3_init(&R4);
    EFp3_init(&R5);
    EFp3_init(&R6);
    EFp3_init(&R7);
    EFp3_init(&ANS_temp);
    EFp3_set_infity(&ANS_temp);
    EFp3_set(&R, P);
    EFp3_ECA(&R2, &R, &R);
    EFp3_ECA(&R3, &R2, &R);
    EFp3_ECA(&R4, &R3, &R);
    EFp3_ECA(&R5, &R4, &R);
    EFp3_ECA(&R6, &R5, &R);
    EFp3_ECA(&R7, &R6, &R);
    count_eca_WIN_map = count_eca_WIN_map + 14;
    
    for(i=0; i< mul3;i=i+3){
        
        EFp3_ECD(&ANS_temp,&ANS_temp);
        EFp3_ECD(&ANS_temp,&ANS_temp);
        EFp3_ECD(&ANS_temp,&ANS_temp);
        count_ecd_WIN_map = count_ecd_WIN_map +  3;
        if (i+2 < length)
        {
            if(r_binary[i] =='1' && r_binary[i+1] =='1' && r_binary[i+2] =='1'){
                EFp3_ECA(&ANS_temp, &ANS_temp, &R7);
                count_eca_WIN_map++;
            }
            else if(r_binary[i] =='1' && r_binary[i+1] =='1' && r_binary[i+2] =='0'){
                EFp3_ECA(&ANS_temp, &ANS_temp, &R6);
                 count_eca_WIN_map++;
            }
            else if(r_binary[i] =='1' && r_binary[i+1] =='0' && r_binary[i+2] =='1'){
                EFp3_ECA(&ANS_temp, &ANS_temp, &R5);
                 count_eca_WIN_map++;
            }
            else if(r_binary[i] =='1' && r_binary[i+1] =='0' && r_binary[i+2] =='0'){
                EFp3_ECA(&ANS_temp, &ANS_temp, &R4);
                 count_eca_WIN_map++;
            }
            else if(r_binary[i] =='0' && r_binary[i+1] =='1' && r_binary[i+2] =='1'){
                EFp3_ECA(&ANS_temp, &ANS_temp, &R3);
                 count_eca_WIN_map++;
            }
            else if(r_binary[i] =='0' && r_binary[i+1] =='1' && r_binary[i+2] =='0'){
                EFp3_ECA(&ANS_temp, &ANS_temp, &R2);
                 count_eca_WIN_map++;
            }
            else if(r_binary[i] =='0' && r_binary[i+1] =='0' && r_binary[i+2] =='1'){
                EFp3_ECA(&ANS_temp, &ANS_temp, &R);
                 count_eca_WIN_map++;
            }
            else{
            }
        }
    }
    
    for(i = mul3;r_binary[i]!='\0';i++){
        EFp3_ECD(&ANS_temp,&ANS_temp);
         count_ecd_WIN_map++;
        if(r_binary[i]=='1'){
            EFp3_ECA(&ANS_temp,&ANS_temp,P);
            count_eca_WIN_map++;
        }
    }
    EFp3_set(ANS,&ANS_temp);
    EFp3_clear(&ANS_temp);
    EFp3_clear(&R);
    EFp3_clear(&R2);
    EFp3_clear(&R3);
    EFp3_clear(&R4);
    EFp3_clear(&R5);
    EFp3_clear(&R6);
    EFp3_clear(&R7);
    return;
}

int EFp3_cmp(struct EFp3 *A,struct EFp3 *B){
    if(Fp3_cmp(&A->x,&B->x)==0 && Fp3_cmp(&A->y,&B->y)==0){
        return 0;
    }
    return 1;
}
void EFp3_set_EFp(struct EFp3 *A,struct EFp *B){
    Fp3_set_ui(&A->x,0);
    Fp3_set_ui(&A->y,0);
    
    Fp_set(&A->x.x0,&B->x);
    Fp_set(&A->y.x0,&B->y);
    A->infity=B->infity;
}
void EFp3_random_set(struct EFp3 *ANS){
    struct Fp3 a,x;
    Fp3_init(&a);
    Fp3_init(&x);
    
    struct EFp3 P,Q;
    EFp3_init(&P);
    EFp3_init(&Q);
    
    mpz_t p3,t3,r3,tmp;
    mpz_init(p3);
    mpz_init(t3);
    mpz_init(r3);
    mpz_init(tmp);
    
    mpz_mul(p3,prime,prime);
    mpz_mul(p3,p3,prime);//p3=p^3
    
    mpz_mul(t3,trace_t,trace_t);
    mpz_mul(t3,t3,trace_t);//t3=t^3
    
    mpz_mul(tmp,trace_t,prime);
    mpz_mul_ui(tmp,tmp,3);//tmp=3tp
    
    mpz_sub(r3,p3,t3);//r3=p3-t3
    mpz_add(r3,r3,tmp);//r3=p3-t3+3tp
    mpz_add_ui(r3,r3,1);//r3=p3-t3+3tp+1
    
    mpz_tdiv_q(r3,r3,order_r);
    mpz_tdiv_q(r3,r3,order_r);
    do{
        Fp3_random(&x);
        Fp3_mul(&a,&x,&x);
        Fp3_mul(&a,&a,&x);
        mpz_add(a.x0.x0,a.x0.x0,b);
    }while(Fp3_legendre(&a)!=1);
    P.infity=FALSE;
    Fp3_sqrt(&P.y,&a);
    Fp3_set(&P.x,&x);
    
    EFp3_SCM_BIN(&P,&P,r3);
    EFp3_set(ANS,&P);
    
    
    Fp3_clear(&a);
    Fp3_clear(&x);
    EFp3_clear(&P);
    mpz_clear(p3);
}
void EFp3_neg(struct EFp3 *ANS, struct EFp3 *A){
    struct EFp3 tmp;
    EFp3_init(&tmp);
    Fp3_neg(&tmp.y,&A->y);
    Fp3_set(&tmp.x,&A->x);
    
    EFp3_set(ANS,&tmp);
    EFp3_clear(&tmp);
}

//-----------------------------------------------------------------------------------------
#pragma mark EFp6 methods
void EFp6_init(struct EFp6 *A){
    Fp6_init(&A->x);
    Fp6_init(&A->y);
    A->infity=FALSE;
}
void EFp6_set(struct EFp6 *A,struct EFp6 *B){
    Fp6_set(&A->x,&B->x);
    Fp6_set(&A->y,&B->y);
    A->infity=B->infity;
}
void EFp6_set_infity(struct EFp6 *A){
    Fp6_set_ui(&A->x,0);
    Fp6_set_ui(&A->y,0);
    A->infity=TRUE;
}
void EFp6_clear(struct EFp6 *A){
    Fp6_clear(&A->x);
    Fp6_clear(&A->y);
}
void EFp6_printf(struct EFp6 *A){
    gmp_printf("%Zd,%Zd,%Zd,%Zd,%Zd,%Zd\n",A->x.x0.x0.x0,A->x.x0.x1.x0,A->x.x0.x2.x0,A->x.x1.x0.x0,A->x.x1.x1.x0,A->x.x1.x2.x0);
    gmp_printf("%Zd,%Zd,%Zd,%Zd,%Zd,%Zd\n",A->y.x0.x0.x0,A->y.x0.x1.x0,A->y.x0.x2.x0,A->y.x1.x0.x0,A->y.x1.x1.x0,A->y.x1.x2.x0);
}
void EFp6_ECD(struct EFp6 *ANS, struct EFp6 *P){
    if(P->infity==TRUE){
        EFp6_set(ANS,P);
        return;
    }
    mpz_t cmp;
    mpz_init(cmp);
    mpz_set_ui(cmp,0);
    if(Fp6_cmp_mpz(&P->y,cmp)==0){//P.y==0
        EFp6_set_infity(ANS);
        return;
    }
    
    struct Fp6 x,y,lambda,tmp;
    struct EFp6 t_ans;
    Fp6_init(&x);
    Fp6_init(&lambda);
    Fp6_init(&tmp);
    Fp6_init(&y);
    EFp6_init(&t_ans);
    
    Fp6_mul(&x,&P->x,&P->x);
    Fp6_add(&tmp,&x,&x);
    Fp6_add(&x,&tmp,&x);
    Fp6_add(&y,&P->y,&P->y);
    Fp6_div(&lambda,&x,&y);
    Fp6_mul(&tmp,&lambda,&lambda);
    Fp6_add(&x,&P->x,&P->x);
    Fp6_sub(&x,&tmp,&x);
    Fp6_sub(&tmp,&P->x,&x);
    Fp6_set(&t_ans.x,&x);
    Fp6_mul(&tmp,&tmp,&lambda);
    Fp6_sub(&t_ans.y,&tmp,&P->y);
    
    EFp6_set(ANS,&t_ans);
    
    Fp6_clear(&x);
    Fp6_clear(&lambda);
    Fp6_clear(&y);
    Fp6_clear(&tmp);
    EFp6_clear(&t_ans);
}
void EFp6_ECA(struct EFp6 *ANS, struct EFp6 *P1, struct EFp6 *P2){
    if(P2->infity==TRUE){//if P2==inf
        EFp6_set(ANS,P1);
        return;
    }
    else if(P1->infity==TRUE){//if P1==inf
        EFp6_set(ANS,P2);
        return;
    }
    else if(Fp6_cmp(&P1->x,&P2->x)==0&&Fp6_cmp(&P1->y,&P2->y)==1){ //P1.x==P2.x&&P1.y!=P2.y
        EFp6_set_infity(ANS);
        return;
    }
    else if(EFp6_cmp(P1,P2)==0){ // P=Q
        EFp6_ECD(ANS,P1);
        return;
    }
    
    struct Fp6 x,y,lambda,tmp;
    struct EFp6 t_ans;
    
    Fp6_init(&x);
    Fp6_init(&y);
    Fp6_init(&lambda);
    Fp6_init(&tmp);
    EFp6_init(&t_ans);
    
    Fp6_sub(&x,&P2->x,&P1->x);
    Fp6_sub(&y,&P2->y,&P1->y);
    Fp6_div(&lambda,&y,&x);
    Fp6_mul(&tmp,&lambda,&lambda);
    Fp6_add(&x,&P1->x,&P2->x);
    Fp6_sub(&x,&tmp,&x);
    Fp6_sub(&tmp,&P1->x,&x);
    Fp6_set(&t_ans.x,&x);
    Fp6_mul(&tmp,&tmp,&lambda);
    Fp6_sub(&t_ans.y,&tmp,&P1->y);
    
    EFp6_set(ANS,&t_ans);
    
    Fp6_clear(&x);
    Fp6_clear(&y);
    Fp6_clear(&lambda);
    Fp6_clear(&tmp);
    EFp6_clear(&t_ans);
}
void EFp6_SCM_BIN(struct EFp6 *ANS, struct EFp6 *P,mpz_t j){
    int i;
    int r;//bit数
    r= (int)mpz_sizeinbase(j,2);
    
    struct EFp6 Q;
    EFp6_init(&Q);
    EFp6_set(&Q,P);
    
    for(i=r-2;i>=0;i--){
        if(mpz_tstbit(j,i)==1){
            EFp6_ECD(&Q,&Q);
            EFp6_ECA(&Q,&Q,P);
        }else{
            EFp6_ECD(&Q,&Q);
        }
    }
    
    EFp6_set(ANS,&Q);
    EFp6_clear(&Q);
    return;
}
//NAF
void EFp6_SCM_NAF(struct EFp6 *ANS, struct EFp6 *P, mpz_t scalar)
{
    TO_NAF(scalar);
    struct EFp6 Q,R;
    EFp6_init(&Q);
    EFp6_set_infity(&Q);
    EFp6_init(&R);
    
    for (int j = 0; j< naf_index; j++){
        //        printf("J %ld\n",myNAF[j]);
        EFp6_ECD(&Q,&Q);
        if(myNAF[j]== 1){
            EFp6_ECA(&Q,&Q,P);
        }
        else if(myNAF[j] == -1){
            EFp6_neg(&R, P);
            EFp6_ECA(&Q,&Q,&R);
        }
    }
    EFp6_set(ANS,&Q);
    EFp6_clear(&Q);
    EFp6_clear(&R);
    return;
}
void EFp6_SCM_WIN(struct EFp6 *ANS, struct EFp6 *P, mpz_t scalar){
    int i,length;
    length= (int)mpz_sizeinbase(scalar,2);
    char r_binary[length];
    mpz_get_str(r_binary,2,scalar);
    
    int rem = length % 3;
    int mul3 = length - rem;
    
    struct EFp6 R,R2,R3,R4,R5,R6,R7,ANS_temp;
    EFp6_init(&R);
    EFp6_init(&R2);
    EFp6_init(&R3);
    EFp6_init(&R4);
    EFp6_init(&R5);
    EFp6_init(&R6);
    EFp6_init(&R7);
    EFp6_init(&ANS_temp);
    EFp6_set_infity(&ANS_temp);
    EFp6_set(&R, P);
    EFp6_ECA(&R2, &R, &R);
    EFp6_ECA(&R3, &R2, &R);
    EFp6_ECA(&R4, &R3, &R);
    EFp6_ECA(&R5, &R4, &R);
    EFp6_ECA(&R6, &R5, &R);
    EFp6_ECA(&R7, &R6, &R);
    
    for(i=0; i< mul3;i=i+3){
        
        EFp6_ECD(&ANS_temp,&ANS_temp);
        EFp6_ECD(&ANS_temp,&ANS_temp);
        EFp6_ECD(&ANS_temp,&ANS_temp);
        
        if (i+2 < length)
        {
            if(r_binary[i] =='1' && r_binary[i+1] =='1' && r_binary[i+2] =='1'){
                EFp6_ECA(&ANS_temp, &ANS_temp, &R7);
            }
            else if(r_binary[i] =='1' && r_binary[i+1] =='1' && r_binary[i+2] =='0'){
                EFp6_ECA(&ANS_temp, &ANS_temp, &R6);
            }
            else if(r_binary[i] =='1' && r_binary[i+1] =='0' && r_binary[i+2] =='1'){
                EFp6_ECA(&ANS_temp, &ANS_temp, &R5);
            }
            else if(r_binary[i] =='1' && r_binary[i+1] =='0' && r_binary[i+2] =='0'){
                EFp6_ECA(&ANS_temp, &ANS_temp, &R4);
            }
            else if(r_binary[i] =='0' && r_binary[i+1] =='1' && r_binary[i+2] =='1'){
                EFp6_ECA(&ANS_temp, &ANS_temp, &R3);
            }
            else if(r_binary[i] =='0' && r_binary[i+1] =='1' && r_binary[i+2] =='0'){
                EFp6_ECA(&ANS_temp, &ANS_temp, &R2);
            }
            else if(r_binary[i] =='0' && r_binary[i+1] =='0' && r_binary[i+2] =='1'){
                EFp6_ECA(&ANS_temp, &ANS_temp, &R);
            }
            else{
            }
        }
    }
    
    for(i = mul3;r_binary[i]!='\0';i++){
        EFp6_ECD(&ANS_temp,&ANS_temp);
        if(r_binary[i]=='1'){
            EFp6_ECA(&ANS_temp,&ANS_temp,P);
        }
    }
    EFp6_set(ANS,&ANS_temp);
    EFp6_clear(&ANS_temp);
    EFp6_clear(&R);
    EFp6_clear(&R2);
    EFp6_clear(&R3);
    EFp6_clear(&R4);
    EFp6_clear(&R5);
    EFp6_clear(&R6);
    EFp6_clear(&R7);
    return;
}

int EFp6_cmp(struct EFp6 *A,struct EFp6 *B){
    if(Fp6_cmp(&A->x,&B->x)==0 && Fp6_cmp(&A->y,&B->y)==0){
        return 0;
    }
    return 1;
}
void EFp6_neg(struct EFp6 *ANS, struct EFp6 *A){
    struct EFp6 tmp;
    EFp6_init(&tmp);
    Fp6_neg(&tmp.y,&A->y);
    Fp6_set(&tmp.x,&A->x);
    
    EFp6_set(ANS,&tmp);
    EFp6_clear(&tmp);
}

void EFp6_random_set(struct EFp6 *ANS){
    struct EFp6 P;
    EFp6_init(&P);
    
    struct Fp6 x,a;
    Fp6_init(&a);
    Fp6_init(&x);
    
    //t12=a^12+b^12={(t^3-3pt)^2-2p^3}^2-2p^6
    mpz_t tmp1,tmp2,p_pow;
    mpz_t r6,t6;
    mpz_init(tmp1);
    mpz_init(tmp2);
    mpz_init(p_pow);
    mpz_init(r6);
    mpz_init(t6);
    
    mpz_pow_ui(tmp1,trace_t,3);
    mpz_mul(p_pow,prime,trace_t);
    mpz_mul_ui(p_pow,p_pow,3);
    mpz_sub(tmp1,tmp1,p_pow);
    mpz_mul(tmp1,tmp1,tmp1);
    
    mpz_pow_ui(p_pow,prime,3);
    mpz_mul_ui(tmp2,p_pow,2);
    mpz_sub(t6,tmp1,tmp2);
    // mpz_pow_ui(tmp1,tmp1,2);
    
    mpz_pow_ui(p_pow,p_pow,2);
    // mpz_mul_ui(tmp2,p_pow,2);
    // mpz_sub(t6,tmp1,tmp2);
    
    // mpz_pow_ui(p_pow,p_pow,2);
    mpz_add_ui(tmp1,p_pow,1);
    mpz_sub(r6,tmp1,t6);
    
    do{
        Fp6_random(&x);
        Fp6_mul(&a,&x,&x);
        Fp6_mul(&a,&a,&x);
        mpz_add(a.x0.x0.x0,a.x0.x0.x0,b);
    }while(Fp6_legendre(&a)!=1);
    Fp6_sqrt(&P.y,&a);
    Fp6_set(&P.x,&x);
    
    mpz_t r6_div_r2;
    mpz_init(r6_div_r2);
    mpz_div(r6_div_r2,r6,order_r);
    mpz_div(r6_div_r2,r6_div_r2,order_r);
    
    EFp6_SCM_BIN(ANS,&P,r6_div_r2);
    
    EFp6_clear(&P);
    Fp6_clear(&a);
    Fp6_clear(&x);
    mpz_clear(tmp1);
    mpz_clear(tmp2);
    mpz_clear(p_pow);
    mpz_clear(r6);
    mpz_clear(t6);
    mpz_clear(r6_div_r2);
}

//-----------------------------------------------------------------------------------------
#pragma mark EFp18 methods
void EFp18_init(struct EFp18 *A){
    Fp18_init(&A->x);
    Fp18_init(&A->y);
    A->infity=FALSE;
}
void EFp18_set(struct EFp18 *A,struct EFp18 *B){
    Fp18_set(&A->x,&B->x);
    Fp18_set(&A->y,&B->y);
    A->infity=B->infity;
}
void EFp18_set_infity(struct EFp18 *A){
    Fp18_set_ui(&A->x,0);
    Fp18_set_ui(&A->y,0);
    A->infity=TRUE;
}
void EFp18_clear(struct EFp18 *A){
    Fp18_clear(&A->x);
    Fp18_clear(&A->y);
}
void EFp18_printf(struct EFp18 *A){
    gmp_printf("(%Zd,%Zd,%Zd,%Zd,%Zd,%Zd\n",A->x.x0.x0.x0.x0,A->x.x0.x0.x1.x0,A->x.x0.x0.x2.x0,A->x.x0.x1.x0.x0,A->x.x0.x1.x1.x0,A->x.x0.x1.x2.x0);
    gmp_printf("%Zd,%Zd,%Zd,%Zd,%Zd,%Zd\n",A->x.x1.x0.x0.x0,A->x.x1.x0.x1.x0,A->x.x1.x0.x2.x0,A->x.x1.x1.x0.x0,A->x.x1.x1.x1.x0,A->x.x1.x1.x2.x0);
    gmp_printf("%Zd,%Zd,%Zd,%Zd,%Zd,%Zd)\n",A->x.x2.x0.x0.x0,A->x.x2.x0.x1.x0,A->x.x2.x0.x2.x0,A->x.x2.x1.x0.x0,A->x.x2.x1.x1.x0,A->x.x2.x1.x2.x0);
    
    gmp_printf("(%Zd,%Zd,%Zd,%Zd,%Zd,%Zd\n",A->y.x0.x0.x0.x0,A->y.x0.x0.x1.x0,A->y.x0.x0.x2.x0,A->y.x0.x1.x0.x0,A->y.x0.x1.x1.x0,A->y.x0.x1.x2.x0);
    gmp_printf("%Zd,%Zd,%Zd,%Zd,%Zd,%Zd\n",A->y.x1.x0.x0.x0,A->y.x1.x0.x1.x0,A->y.x1.x0.x2.x0,A->y.x1.x1.x0.x0,A->y.x1.x1.x1.x0,A->y.x1.x1.x2.x0);
    gmp_printf("%Zd,%Zd,%Zd,%Zd,%Zd,%Zd)\n\n",A->y.x2.x0.x0.x0,A->y.x2.x0.x1.x0,A->y.x2.x0.x2.x0,A->y.x2.x1.x0.x0,A->y.x2.x1.x1.x0,A->y.x2.x1.x2.x0);
}
void EFp18_ECD(struct EFp18 *ANS, struct EFp18 *P){
    if(P->infity==TRUE){
        EFp18_set(ANS,P);
        return;
    }
    mpz_t cmp;
    mpz_init(cmp);
    mpz_set_ui(cmp,0);
    if(Fp18_cmp_mpz(&P->y,cmp)==0){//P.y==0
        EFp18_set_infity(ANS);
        return;
    }
    
    struct Fp18 x,y,lambda,tmp;
    struct EFp18 t_ans;
    Fp18_init(&x);
    Fp18_init(&lambda);
    Fp18_init(&tmp);
    Fp18_init(&y);
    EFp18_init(&t_ans);
    
    Fp18_mul(&x,&P->x,&P->x);
    Fp18_add(&tmp,&x,&x);
    Fp18_add(&x,&tmp,&x);
    Fp18_add(&y,&P->y,&P->y);
    Fp18_div(&lambda,&x,&y);
    Fp18_mul(&tmp,&lambda,&lambda);
    Fp18_add(&x,&P->x,&P->x);
    Fp18_sub(&x,&tmp,&x);
    Fp18_sub(&tmp,&P->x,&x);
    Fp18_set(&t_ans.x,&x);
    Fp18_mul(&tmp,&tmp,&lambda);
    Fp18_sub(&t_ans.y,&tmp,&P->y);
    
    EFp18_set(ANS,&t_ans);
    
    Fp18_clear(&x);
    Fp18_clear(&lambda);
    Fp18_clear(&y);
    Fp18_clear(&tmp);
    EFp18_clear(&t_ans);
}
void EFp18_ECA(struct EFp18 *ANS, struct EFp18 *P1, struct EFp18 *P2){
    if(P2->infity==TRUE){//if P2==inf
        EFp18_set(ANS,P1);
        return;
    }
    else if(P1->infity==TRUE){//if P1==inf
        EFp18_set(ANS,P2);
        return;
    }
    else if(Fp18_cmp(&P1->x,&P2->x)==0&&Fp18_cmp(&P1->y,&P2->y)==1){ //P1.x==P2.x&&P1.y!=P2.y
        EFp18_set_infity(ANS);
        return;
    }
    else if(EFp18_cmp(P1,P2)==0){ // P=Q
        EFp18_ECD(ANS,P1);
        return;
    }
    
    struct Fp18 x,y,lambda,tmp;
    struct EFp18 t_ans;
    
    Fp18_init(&x);
    Fp18_init(&y);
    Fp18_init(&lambda);
    Fp18_init(&tmp);
    EFp18_init(&t_ans);
    
    Fp18_sub(&x,&P2->x,&P1->x);
    Fp18_sub(&y,&P2->y,&P1->y);
    Fp18_div(&lambda,&y,&x);
    Fp18_mul(&tmp,&lambda,&lambda);
    Fp18_add(&x,&P1->x,&P2->x);
    Fp18_sub(&x,&tmp,&x);
    Fp18_sub(&tmp,&P1->x,&x);
    Fp18_set(&t_ans.x,&x);
    Fp18_mul(&tmp,&tmp,&lambda);
    Fp18_sub(&t_ans.y,&tmp,&P1->y);
    
    EFp18_set(ANS,&t_ans);
    
    Fp18_clear(&x);
    Fp18_clear(&y);
    Fp18_clear(&lambda);
    Fp18_clear(&tmp);
    EFp18_clear(&t_ans);
}
void EFp18_SCM_BIN(struct EFp18 *ANS, struct EFp18 *P,mpz_t j){
    int i;
    int r;//bit数
    r= (int)mpz_sizeinbase(j,2);
    count_eca_BIN=0; count_ecd_BIN=0;
    struct EFp18 Q;
    EFp18_init(&Q);
    EFp18_set(&Q,P);
    
    for(i=r-2;i>=0;i--){
        if(mpz_tstbit(j,i)==1){
            EFp18_ECD(&Q,&Q);
            EFp18_ECA(&Q,&Q,P);
            count_eca_BIN++;
        }else{
            EFp18_ECD(&Q,&Q);
        }
        count_ecd_BIN++;
        //        printf("I am in old \n");
    }
    
    EFp18_set(ANS,&Q);
    EFp18_clear(&Q);
    return;
}
//NAF
void EFp18_SCM_NAF(struct EFp18 *ANS, struct EFp18 *P, mpz_t scalar)
{
    count_eca_NAF = 0; count_ecd_NAF = 0;
    TO_NAF(scalar);
    struct EFp18 Q,R;
    EFp18_init(&Q);
    EFp18_set_infity(&Q);
    EFp18_init(&R);
    
    for (int j = 0; j< naf_index; j++){
        EFp18_ECD(&Q,&Q);
        count_ecd_NAF++;
        
        if(myNAF[j]== 1){
            EFp18_ECA(&Q,&Q,P);
            count_eca_NAF ++;
        }
        else if(myNAF[j] == -1){
            EFp18_neg(&R, P);
            EFp18_ECA(&Q,&Q,&R);
            count_eca_NAF ++;
        }
    }
    EFp18_set(ANS,&Q);
    EFp18_clear(&Q);
    EFp18_clear(&R);
    return;
}
void EFp3_SCM_WIN(struct EFp3 *ANS, struct EFp3 *P, mpz_t scalar){
    int i,length;
    length= (int)mpz_sizeinbase(scalar,2);
    char r_binary[length];
    mpz_get_str(r_binary,2,scalar);
    
    int win_size = 4;
    int rem = length % win_size;
    int mul3 = length - rem;
    
    count_eca_WIN_map = 0; count_ecd_WIN_map = 0;
    
    gettimeofday(&tonaf_t0, 0);
    
    struct EFp3 R,R2,R3,R4,R5,R6,R7,R8,R9,R10,R11,R12,R13,R14,R15,ANS_temp;
    EFp3_init(&R);
    EFp3_init(&R2);
    EFp3_init(&R3);
    EFp3_init(&R4);
    EFp3_init(&R5);
    EFp3_init(&R6);
    EFp3_init(&R7);
    EFp3_init(&R8);
    EFp3_init(&R9);
    EFp3_init(&R10);
    EFp3_init(&R11);
    EFp3_init(&R12);
    EFp3_init(&R13);
    EFp3_init(&R14);
    EFp3_init(&R15);
    EFp3_init(&ANS_temp);
    
    EFp3_set_infity(&ANS_temp);
    EFp3_set(&R, P);
    EFp3_ECA(&R2, &R, &R);
    EFp3_ECA(&R3, &R2, &R);
    EFp3_ECA(&R4, &R3, &R);
    EFp3_ECA(&R5, &R4, &R);
    EFp3_ECA(&R6, &R5, &R);
    EFp3_ECA(&R7, &R6, &R);
    EFp3_ECA(&R8, &R7, &R);
    EFp3_ECA(&R9, &R8, &R);
    EFp3_ECA(&R10, &R9, &R);
    EFp3_ECA(&R11, &R10, &R);
    EFp3_ECA(&R12, &R11, &R);
    EFp3_ECA(&R13, &R12, &R);
    EFp3_ECA(&R14, &R13, &R);
    EFp3_ECA(&R15, &R14, &R);
    gettimeofday(&tonaf_t1, 0);
    
    elapsed_tonaf_t0 = timedifference_msec(tonaf_t0, tonaf_t1);
    printf("WIN:: 14 ECA precalculation =  %f milliseconds.\n", elapsed_tonaf_t0);
    
    count_eca_WIN_map =  count_eca_WIN_map + 14;
    
    for(i=0; i< mul3;i=i+4){
        
        EFp3_ECD(&ANS_temp,&ANS_temp);
        EFp3_ECD(&ANS_temp,&ANS_temp);
        EFp3_ECD(&ANS_temp,&ANS_temp);
        EFp3_ECD(&ANS_temp,&ANS_temp);
        count_ecd_WIN_map = count_ecd_WIN_map + 4;
        
        if (i+2 < length)
        {
            if(r_binary[i] =='1' && r_binary[i+1] =='1' && r_binary[i+2] =='1' && r_binary[i+3] =='1'){
                EFp3_ECA(&ANS_temp, &ANS_temp, &R15);
                count_eca_WIN_map ++;
            }
            else if(r_binary[i] =='1' && r_binary[i+1] =='1' && r_binary[i+2] =='1' && r_binary[i+3] =='0'){
                EFp3_ECA(&ANS_temp, &ANS_temp, &R14);
                count_eca_WIN_map ++;
            }
            else if(r_binary[i] =='1' && r_binary[i+1] =='1' && r_binary[i+2] =='0' && r_binary[i+3] =='1'){
                EFp3_ECA(&ANS_temp, &ANS_temp, &R13);
                count_eca_WIN_map ++;
            }
            else if(r_binary[i] =='1' && r_binary[i+1] =='1' && r_binary[i+2] =='0' && r_binary[i+3] =='0'){
                EFp3_ECA(&ANS_temp, &ANS_temp, &R12);
                count_eca_WIN_map ++;
            }
            else if(r_binary[i] =='1' && r_binary[i+1] =='0' && r_binary[i+2] =='1' && r_binary[i+3] =='1'){
                EFp3_ECA(&ANS_temp, &ANS_temp, &R11);
                count_eca_WIN_map ++;
            }
            else if(r_binary[i] =='1' && r_binary[i+1] =='0' && r_binary[i+2] =='1' && r_binary[i+3] =='0'){
                EFp3_ECA(&ANS_temp, &ANS_temp, &R10);
                count_eca_WIN_map ++;
            }
            else if(r_binary[i] =='1' && r_binary[i+1] =='0' && r_binary[i+2] =='0' && r_binary[i+3] =='1'){
                EFp3_ECA(&ANS_temp, &ANS_temp, &R9);
                count_eca_WIN_map ++;
            }
            else if(r_binary[i] =='1' && r_binary[i+1] =='0' && r_binary[i+2] =='0' && r_binary[i+3] =='0'){
                EFp3_ECA(&ANS_temp, &ANS_temp, &R8);
                count_eca_WIN_map ++;
            }
            else if(r_binary[i] =='0' && r_binary[i+1] =='1' && r_binary[i+2] =='1' && r_binary[i+3] =='1'){
                EFp3_ECA(&ANS_temp, &ANS_temp, &R7);
                count_eca_WIN_map ++;
            }
            else if(r_binary[i] =='0' && r_binary[i+1] =='1' && r_binary[i+2] =='1' && r_binary[i+3] =='0'){
                EFp3_ECA(&ANS_temp, &ANS_temp, &R6);
                count_eca_WIN_map ++;
            }
            else if(r_binary[i] =='0' && r_binary[i+1] =='1' && r_binary[i+2] =='0' && r_binary[i+3] =='1'){
                EFp3_ECA(&ANS_temp, &ANS_temp, &R5);
                count_eca_WIN_map ++;
            }
            else if(r_binary[i] =='0' && r_binary[i+1] =='1' && r_binary[i+2] =='0' && r_binary[i+3] =='0'){
                EFp3_ECA(&ANS_temp, &ANS_temp, &R4);
                count_eca_WIN_map ++;
            }
            else if(r_binary[i] =='0' && r_binary[i+1] =='0' && r_binary[i+2] =='1' && r_binary[i+3] =='1'){
                EFp3_ECA(&ANS_temp, &ANS_temp, &R3);
                count_eca_WIN_map ++;
            }
            else if(r_binary[i] =='0' && r_binary[i+1] =='0' && r_binary[i+2] =='1' && r_binary[i+3] =='0'){
                EFp3_ECA(&ANS_temp, &ANS_temp, &R2);
                count_eca_WIN_map ++;
            }
            else if(r_binary[i] =='0' && r_binary[i+1] =='0' && r_binary[i+2] =='0' && r_binary[i+3] =='1'){
                EFp3_ECA(&ANS_temp, &ANS_temp, &R);
                count_eca_WIN_map ++;
            }
            else{
            }
        }
    }
    
    for(i = mul3;r_binary[i]!='\0';i++){
        EFp3_ECD(&ANS_temp,&ANS_temp);
        count_ecd_WIN_map = count_ecd_WIN_map + 1;
        if(r_binary[i]=='1'){
            EFp3_ECA(&ANS_temp,&ANS_temp,P);
            count_eca_WIN_map ++;
        }
    }
    EFp3_set(ANS,&ANS_temp);
    EFp3_clear(&ANS_temp);
    EFp3_clear(&R);
    EFp3_clear(&R2);
    EFp3_clear(&R3);
    EFp3_clear(&R4);
    EFp3_clear(&R5);
    EFp3_clear(&R6);
    EFp3_clear(&R7);
    EFp3_clear(&R8);
    EFp3_clear(&R9);
    EFp3_clear(&R10);
    EFp3_clear(&R11);
    EFp3_clear(&R12);
    EFp3_clear(&R13);
    EFp3_clear(&R14);
    EFp3_clear(&R15);
    return;
}

void EFp18_SCM_WIN(struct EFp18 *ANS, struct EFp18 *P, mpz_t scalar){
    int i,length;
    length= (int)mpz_sizeinbase(scalar,2);
    char r_binary[length];
    mpz_get_str(r_binary,2,scalar);
    
    int win_size = 4;
    int rem = length % win_size;
    int mul3 = length - rem;
    
    count_eca_WIN = 0; count_ecd_WIN = 0;
    
    gettimeofday(&tonaf_t0, 0);
    
    struct EFp18 R,R2,R3,R4,R5,R6,R7,R8,R9,R10,R11,R12,R13,R14,R15,ANS_temp;
    EFp18_init(&R);
    EFp18_init(&R2);
    EFp18_init(&R3);
    EFp18_init(&R4);
    EFp18_init(&R5);
    EFp18_init(&R6);
    EFp18_init(&R7);
    EFp18_init(&R8);
    EFp18_init(&R9);
    EFp18_init(&R10);
    EFp18_init(&R11);
    EFp18_init(&R12);
    EFp18_init(&R13);
    EFp18_init(&R14);
    EFp18_init(&R15);
    EFp18_init(&ANS_temp);
    
    EFp18_set_infity(&ANS_temp);
    EFp18_set(&R, P);
    EFp18_ECA(&R2, &R, &R);
    EFp18_ECA(&R3, &R2, &R);
    EFp18_ECA(&R4, &R3, &R);
    EFp18_ECA(&R5, &R4, &R);
    EFp18_ECA(&R6, &R5, &R);
    EFp18_ECA(&R7, &R6, &R);
    EFp18_ECA(&R8, &R7, &R);
    EFp18_ECA(&R9, &R8, &R);
    EFp18_ECA(&R10, &R9, &R);
    EFp18_ECA(&R11, &R10, &R);
    EFp18_ECA(&R12, &R11, &R);
    EFp18_ECA(&R13, &R12, &R);
    EFp18_ECA(&R14, &R13, &R);
    EFp18_ECA(&R15, &R14, &R);
    gettimeofday(&tonaf_t1, 0);
    
    elapsed_tonaf_t0 = timedifference_msec(tonaf_t0, tonaf_t1);
    printf("WIN:: 14 ECA precalculation =  %f milliseconds.\n", elapsed_tonaf_t0);
    
    count_eca_WIN =  count_eca_WIN + 14;
    
    for(i=0; i< mul3;i=i+4){
        
        EFp18_ECD(&ANS_temp,&ANS_temp);
        EFp18_ECD(&ANS_temp,&ANS_temp);
        EFp18_ECD(&ANS_temp,&ANS_temp);
        EFp18_ECD(&ANS_temp,&ANS_temp);
        count_ecd_WIN = count_ecd_WIN + 4;
        
        if (i+2 < length)
        {
            if(r_binary[i] =='1' && r_binary[i+1] =='1' && r_binary[i+2] =='1' && r_binary[i+3] =='1'){
                EFp18_ECA(&ANS_temp, &ANS_temp, &R15);
                count_eca_WIN ++;
            }
            else if(r_binary[i] =='1' && r_binary[i+1] =='1' && r_binary[i+2] =='1' && r_binary[i+3] =='0'){
                EFp18_ECA(&ANS_temp, &ANS_temp, &R14);
                count_eca_WIN ++;
            }
            else if(r_binary[i] =='1' && r_binary[i+1] =='1' && r_binary[i+2] =='0' && r_binary[i+3] =='1'){
                EFp18_ECA(&ANS_temp, &ANS_temp, &R13);
                count_eca_WIN ++;
            }
            else if(r_binary[i] =='1' && r_binary[i+1] =='1' && r_binary[i+2] =='0' && r_binary[i+3] =='0'){
                EFp18_ECA(&ANS_temp, &ANS_temp, &R12);
                count_eca_WIN ++;
            }
            else if(r_binary[i] =='1' && r_binary[i+1] =='0' && r_binary[i+2] =='1' && r_binary[i+3] =='1'){
                EFp18_ECA(&ANS_temp, &ANS_temp, &R11);
                count_eca_WIN ++;
            }
            else if(r_binary[i] =='1' && r_binary[i+1] =='0' && r_binary[i+2] =='1' && r_binary[i+3] =='0'){
                EFp18_ECA(&ANS_temp, &ANS_temp, &R10);
                count_eca_WIN ++;
            }
            else if(r_binary[i] =='1' && r_binary[i+1] =='0' && r_binary[i+2] =='0' && r_binary[i+3] =='1'){
                EFp18_ECA(&ANS_temp, &ANS_temp, &R9);
                count_eca_WIN ++;
            }
            else if(r_binary[i] =='1' && r_binary[i+1] =='0' && r_binary[i+2] =='0' && r_binary[i+3] =='0'){
                EFp18_ECA(&ANS_temp, &ANS_temp, &R8);
                count_eca_WIN ++;
            }
            else if(r_binary[i] =='0' && r_binary[i+1] =='1' && r_binary[i+2] =='1' && r_binary[i+3] =='1'){
                EFp18_ECA(&ANS_temp, &ANS_temp, &R7);
                count_eca_WIN ++;
            }
            else if(r_binary[i] =='0' && r_binary[i+1] =='1' && r_binary[i+2] =='1' && r_binary[i+3] =='0'){
                EFp18_ECA(&ANS_temp, &ANS_temp, &R6);
                count_eca_WIN ++;
            }
            else if(r_binary[i] =='0' && r_binary[i+1] =='1' && r_binary[i+2] =='0' && r_binary[i+3] =='1'){
                EFp18_ECA(&ANS_temp, &ANS_temp, &R5);
                count_eca_WIN ++;
            }
            else if(r_binary[i] =='0' && r_binary[i+1] =='1' && r_binary[i+2] =='0' && r_binary[i+3] =='0'){
                EFp18_ECA(&ANS_temp, &ANS_temp, &R4);
                count_eca_WIN ++;
            }
            else if(r_binary[i] =='0' && r_binary[i+1] =='0' && r_binary[i+2] =='1' && r_binary[i+3] =='1'){
                EFp18_ECA(&ANS_temp, &ANS_temp, &R3);
                count_eca_WIN ++;
            }
            else if(r_binary[i] =='0' && r_binary[i+1] =='0' && r_binary[i+2] =='1' && r_binary[i+3] =='0'){
                EFp18_ECA(&ANS_temp, &ANS_temp, &R2);
                count_eca_WIN ++;
            }
            else if(r_binary[i] =='0' && r_binary[i+1] =='0' && r_binary[i+2] =='0' && r_binary[i+3] =='1'){
                EFp18_ECA(&ANS_temp, &ANS_temp, &R);
                count_eca_WIN ++;
            }
            else{
            }
        }
    }
    
    for(i = mul3;r_binary[i]!='\0';i++){
        EFp18_ECD(&ANS_temp,&ANS_temp);
        count_ecd_WIN = count_ecd_WIN + 1;
        if(r_binary[i]=='1'){
            EFp18_ECA(&ANS_temp,&ANS_temp,P);
            count_eca_WIN ++;
        }
    }
    EFp18_set(ANS,&ANS_temp);
    EFp18_clear(&ANS_temp);
    EFp18_clear(&R);
    EFp18_clear(&R2);
    EFp18_clear(&R3);
    EFp18_clear(&R4);
    EFp18_clear(&R5);
    EFp18_clear(&R6);
    EFp18_clear(&R7);
    EFp18_clear(&R8);
    EFp18_clear(&R9);
    EFp18_clear(&R10);
    EFp18_clear(&R11);
    EFp18_clear(&R12);
    EFp18_clear(&R13);
    EFp18_clear(&R14);
    EFp18_clear(&R15);
    return;
}
int EFp18_cmp(struct EFp18 *A,struct EFp18 *B){
    if(Fp18_cmp(&A->x,&B->x)==0 && Fp18_cmp(&A->y,&B->y)==0){
        return 0;
    }
    return 1;
}
void EFp18_neg(struct EFp18 *ANS, struct EFp18 *A){
    struct EFp18 tmp;
    EFp18_init(&tmp);
    Fp18_neg(&tmp.y,&A->y);
    Fp18_set(&tmp.x,&A->x);
    
    EFp18_set(ANS,&tmp);
    EFp18_clear(&tmp);
}

void EFp18_random_set(struct EFp18 *ANS){
    struct EFp18 P,ans_temp;
    EFp18_init(&P);
    EFp18_init(&ans_temp);
    
    struct Fp18 x,a;
    Fp18_init(&a);
    Fp18_init(&x);
    
    //t18=a^18+b^18={(t^3-3pt)^3-3p^3(t^3-3pt)}^2-2p^9
    mpz_t tmp1,tmp2,tmp3,t_ans,t18,r18;
    mpz_init(tmp1);
    mpz_init(tmp2);
    mpz_init(tmp3);
    mpz_init(t_ans);
    mpz_init(t18);
    mpz_init(r18);
    
    mpz_mul(tmp1,prime,trace_t);
    mpz_mul_ui(tmp1,tmp1,3);
    
    mpz_mul(tmp2,trace_t,trace_t);
    mpz_mul(tmp2,tmp2,trace_t);
    
    mpz_sub(tmp1,tmp2,tmp1);//tmp1=t^3-3pt
    
    mpz_mul(t_ans,tmp1,tmp1);
    mpz_mul(t_ans,t_ans,tmp1);//t_ans=(t^3-3pt)^3
    
    mpz_mul(tmp2,prime,prime);
    mpz_mul(tmp2,tmp2,prime);//tmp2=p^3
    
    mpz_mul(tmp3,tmp1,tmp2);
    mpz_mul_ui(tmp3,tmp3,3);//tmp3=3p^3(t^3-3pt)
    
    mpz_sub(t_ans,t_ans,tmp3);//t_ans=(t^3-3pt)^3-3p^3(t^3-3pt)
    mpz_mul(t_ans,t_ans,t_ans);//t_ans=t_ans^2
    
    mpz_pow_ui(tmp2,tmp2,3);//tmp2=p^9
    mpz_add(tmp3,tmp2,tmp2);//tmp3=2*tmp2
    
    mpz_sub(t18,t_ans,tmp3);//t18=t_ans-tmp3
    
    mpz_mul(tmp3,tmp2,tmp2);//tmp3=p^18
    
    mpz_add_ui(tmp3,tmp3,1);
    mpz_sub(r18,tmp3,t18);
    mpz_div(r18,r18,order_r);
    mpz_div(r18,r18,order_r);
    
    do{
        Fp18_random(&x);
        Fp18_mul(&a,&x,&x);
        Fp18_mul(&a,&a,&x);
        mpz_add(a.x0.x0.x0.x0,a.x0.x0.x0.x0,b);
    }while(Fp18_legendre(&a)!=1);
    Fp18_sqrt(&P.y,&a);
    Fp18_set(&P.x,&x);
    EFp18_SCM_BIN(ANS,&P,r18);//R
    
    EFp18_SCM_BIN(&ans_temp, ANS, order_r);//T
    if (ans_temp.infity == TRUE)
    {
        printf("Check Successful \n");
    }
    
    Fp18_clear(&a);
    Fp18_clear(&x);
    mpz_clear(tmp1);
    mpz_clear(tmp2);
    mpz_clear(tmp3);
    mpz_clear(t_ans);
    mpz_clear(t18);
    mpz_clear(r18);
}

void get_precalc_frobenious_mapped_points_in_EFp18(struct EFp18 *ANS){
    struct EFp18 tmp,tmp1;
    EFp18_init(&tmp);
    EFp18_init(&tmp1);
    
    EFp18_init(&Q1);
    EFp18_init(&Q2);
    EFp18_init(&ZQ);
    EFp18_init(&ZQ1);
    EFp18_init(&ZQ2);
    EFp18_init(&QQ1);
    EFp18_init(&QQ2);
    EFp18_init(&Q1Q2);
    EFp18_init(&QQ1Q2);
    EFp18_init(&ZQZQ1);
    EFp18_init(&ZQZQ2);
    EFp18_init(&ZQ1ZQ2);
    EFp18_init(&ZQZQ1ZQ2);
    
    
    //    //FM(T)-T=Q
    //    EFp18_frobenius_map(&tmp, T);//FM(T)
    //    Fp18_set(&tmp1.x,&T->x);
    //    Fp18_neg(&tmp1.y,&T->y);
    //    EFp18_ECA(ANS, &tmp, &tmp1);//FM(T)-T= Q        //ANS = Q
    //    count_eca_pre++;
    //
    //    EFp18_set(&QinG2,ANS);
    
    EFp18_frobenius_map(&phiQ, &QinG2);
    EFp18_set(&Q2,&phiQ);                           //Q2=phi(Q)
    EFp18_SCM_BIN(&PscmQ, &QinG2, prime);
    
    mpz_t sclr_tmp;
    mpz_init(sclr_tmp);
    mpz_set_ui(sclr_tmp,8);
    EFp18_SCM_BIN(&tmp, &QinG2, sclr_tmp); //8Q
    EFp18_frobenius_map(&tmp1, &tmp);               //phi(8Q)
    EFp18_frobenius_map(&Q1, &tmp1);               //phi^2(8Q)
    
    mpz_set_ui(sclr_tmp,5);
    EFp18_SCM_BIN(&tmp, &QinG2, sclr_tmp);                 //5Q
    EFp18_frobenius_map(&tmp1, &tmp);               //phi(5Q)
    EFp18_frobenius_map(&tmp, &tmp1);
    EFp18_frobenius_map(&tmp1, &tmp);
    EFp18_frobenius_map(&tmp, &tmp1);
    EFp18_frobenius_map(&tmp1, &tmp);
    Fp18_neg(&tmp1.y,&tmp1.y);
    EFp18_ECA(&Q1, &Q1, &tmp1);                     //Q1=phi^2(8Q)-phi^5(5Q)
    count_eca_new_pre++;
    
    
    EFp18_ECA(&QQ1,&QinG2,&Q1);
    EFp18_ECA(&QQ2,&QinG2,&Q2);
    EFp18_ECA(&Q1Q2,&Q1,&Q2);
    EFp18_ECA(&QQ1Q2,&QinG2,&Q1Q2);
    count_eca_new_pre = count_eca_new_pre + 4;
    
    mpz_set_ui(sclr_tmp,3);                         //ZQ
    EFp18_SCM_BIN(&tmp, &QinG2, sclr_tmp);
    Fp18_neg(&tmp.y,&tmp.y);
    EFp18_frobenius_map(&ZQ, &tmp);
    
    EFp18_frobenius_map(&tmp, &QinG2);
    EFp18_frobenius_map(&tmp1, &tmp);
    EFp18_frobenius_map(&tmp, &tmp1);
    EFp18_frobenius_map(&tmp1, &tmp);
    EFp18_ECA(&ZQ, &ZQ, &tmp1);                     //ZQ=phi(-3Q)+phi^4(Q)
    count_eca_new_pre = count_eca_new_pre + 1;
    
    EFp18_SCM_BIN(&tmp, &Q1, sclr_tmp);                 //ZQ1
    Fp18_neg(&tmp.y,&tmp.y);
    EFp18_frobenius_map(&ZQ1, &tmp);
    
    EFp18_frobenius_map(&tmp, &Q1);
    EFp18_frobenius_map(&tmp1, &tmp);
    EFp18_frobenius_map(&tmp, &tmp1);
    EFp18_frobenius_map(&tmp1, &tmp);
    EFp18_ECA(&ZQ1, &ZQ1, &tmp1);                     //ZQ1=phi(-3Q1)+phi^4(Q1)
    count_eca_new_pre = count_eca_new_pre + 1;
    
    EFp18_SCM_BIN(&tmp, &Q2, sclr_tmp);                 //ZQ2
    Fp18_neg(&tmp.y,&tmp.y);
    EFp18_frobenius_map(&ZQ2, &tmp);
    
    EFp18_frobenius_map(&tmp, &Q2);
    EFp18_frobenius_map(&tmp1, &tmp);
    EFp18_frobenius_map(&tmp, &tmp1);
    EFp18_frobenius_map(&tmp1, &tmp);
    EFp18_ECA(&ZQ2, &ZQ2, &tmp1);                     //ZQ2=phi(-3Q2)+phi^4(Q2)
    count_eca_new_pre = count_eca_new_pre + 1;
    
    EFp18_ECA(&ZQZQ1,&ZQ,&ZQ1);
    EFp18_ECA(&ZQZQ2,&ZQ,&ZQ2);
    EFp18_ECA(&ZQ1ZQ2,&ZQ1,&ZQ2);
    EFp18_ECA(&ZQZQ1ZQ2,&ZQ,&ZQ1ZQ2);
    count_eca_new_pre = count_eca_new_pre + 4;
    
    printf("Fm = \n");
    EFp18_printf(&phiQ);
    //    printf("SM\n");
    //    EFp18_printf(&PscmQ);
    
    if (Fp18_cmp(&PscmQ.x, &phiQ.x) == 0 || Fp18_cmp(&PscmQ.y, &phiQ.y) == 0  )
    {
        printf("Fm is eql SM\n\n");
    }
    
    EFp18_set(ANS,&tmp);
    EFp18_clear(&tmp);
    EFp18_clear(&tmp1);
    mpz_clear(sclr_tmp);
}

void get_z_adic(mpz_t S){
    int length_S0,length_S1,length_S2,length_S3,length_S4,length_S5;
    mpz_t t_1,S1bar,S0bar;
    mpz_t q_tmp1,q_tmp2;
    
    mpz_init(S0bar);
    mpz_init(S1bar);
    mpz_init(x1);
    mpz_init(s0);
    mpz_init(s1);
    mpz_init(s2);
    mpz_init(s3);
    mpz_init(s4);
    mpz_init(s5);
    mpz_init(q_tmp1);
    mpz_init(q_tmp2);
    
    mpz_init(S0X);
    mpz_init(S1X);
    mpz_init(S2X);
    mpz_init(S3X);
    mpz_init(S4X);
    mpz_init(S5X);
    
    mpz_init(t_1);
    mpz_sub_ui(t_1,trace_t,1);
    
    mpz_tdiv_qr(S1bar,S0bar,S,t_1);
    //   gmp_printf("S0bar = %Zd,S1bar =%Zd, t-1=%Zd\n",S0bar,S1bar,t_1);
    
    
    mpz_tdiv_qr(s5,s4,S1bar,X);
    //    gmp_printf("S5 = %Zd,S4 = %Zd\n",s5,s4);
    
    mpz_tdiv_qr(q_tmp1,s0,S0bar,X);
    mpz_tdiv_qr(q_tmp2,s1,q_tmp1,X);
    mpz_tdiv_qr(s3,s2,q_tmp2,X);
    
    
    length_S0= (int)mpz_sizeinbase(s0,2);
    length_S1= (int)mpz_sizeinbase(s1,2);
    length_S2= (int)mpz_sizeinbase(s2,2);
    length_S3= (int)mpz_sizeinbase(s3,2);
    length_S4= (int)mpz_sizeinbase(s4,2);
    length_S5= (int)mpz_sizeinbase(s5,2);
    
    
    int s_size[6] = {length_S0,length_S1,length_S2,length_S3,length_S4,length_S5};
    int max = s_size[0];
    int c = 0, location = 0 ;
    for (c = 0; c < 6; c++){
        if (s_size[c] > max){
            max  = s_size[c];
            location = c;
        }
    }
    
    mpz_setbit(x1,max-1);
    
    mpz_clear(t_1);
    mpz_clear(S1bar);
    mpz_clear(S0bar);
    mpz_clear(q_tmp1);
    mpz_clear(q_tmp2);
}

void EFp18_SCM_New(struct EFp18 *ANS, struct EFp18 *P, mpz_t S){
    
    int Z_state,Q_state;
    struct EFp18 tmp_ans;
    EFp18_init(&tmp_ans);
    EFp18_set_infity(&tmp_ans);
    
    //    char x1_binary[(int)mpz_sizeinbase(x1,2)];
    //    mpz_get_str(x1_binary,2,x1);
    //    printf("x1 binary %s\n",x1_binary);
    
    int flag_1st_ECD = 1;
    
    while (mpz_cmp_ui(x1, 0) > 0)
    {
        mpz_and(S1X,x1,s1);
        mpz_and(S3X,x1,s3);
        mpz_and(S5X,x1,s5);
        
        if (!flag_1st_ECD)
        {
            EFp18_ECD(&tmp_ans, &tmp_ans);
            count_ecd_new++;
        }
        
        
        if (mpz_cmp_ui(S1X, 0) > 0 && mpz_cmp_ui(S3X,0) > 0 && mpz_cmp_ui(S5X,0) > 0)//111
        {
            Z_state = 7;
        }
        else if (mpz_cmp_ui(S1X, 0) > 0 && mpz_cmp_ui(S3X,0) > 0 && mpz_cmp_ui(S5X,0) == 0)//110
        {
            Z_state = 6;
        }
        else if (mpz_cmp_ui(S1X, 0) > 0 && mpz_cmp_ui(S3X,0) == 0 && mpz_cmp_ui(S5X,0) > 0)//101
        {
            Z_state = 5;
        }
        else if (mpz_cmp_ui(S1X, 0) > 0 && mpz_cmp_ui(S3X,0) == 0 && mpz_cmp_ui(S5X,0) == 0)//100
        {
            Z_state = 4;
        }
        else if (mpz_cmp_ui(S1X, 0) == 0 && mpz_cmp_ui(S3X,0) > 0 && mpz_cmp_ui(S5X,0) > 0)//011
        {
            Z_state = 3;
        }
        else if (mpz_cmp_ui(S1X, 0) == 0 && mpz_cmp_ui(S3X,0) > 0 && mpz_cmp_ui(S5X,0) == 0)//010
        {
            Z_state = 2;
        }
        else if (mpz_cmp_ui(S1X, 0) == 0 && mpz_cmp_ui(S3X,0) == 0 && mpz_cmp_ui(S5X,0) > 0)//001
        {
            Z_state = 1;
        }
        else
        {
            Z_state = 0;
        }
        
        
        switch (Z_state) {
            case 7:
                EFp18_ECA(&tmp_ans, &tmp_ans, &ZQZQ1ZQ2);
                count_eca_new++;
                break;
            case 6:
                EFp18_ECA(&tmp_ans, &tmp_ans, &ZQZQ1);
                count_eca_new++;
                break;
            case 5:
                EFp18_ECA(&tmp_ans, &tmp_ans, &ZQZQ2);
                count_eca_new++;
                break;
            case 4:
                EFp18_ECA(&tmp_ans, &tmp_ans, &ZQ);
                count_eca_new++;
                break;
            case 3:
                EFp18_ECA(&tmp_ans, &tmp_ans, &ZQ1ZQ2);
                count_eca_new++;
                break;
            case 2:
                EFp18_ECA(&tmp_ans, &tmp_ans, &ZQ1);
                count_eca_new++;
                break;
            case 1:
                EFp18_ECA(&tmp_ans, &tmp_ans, &ZQ2);
                count_eca_new++;
                break;
            case 0:
                break;
        }
        
        mpz_and(S0X,x1,s0);
        mpz_and(S2X,x1,s2);
        mpz_and(S4X,x1,s4);
        
        
        if (mpz_cmp_ui(S0X, 0) > 0 && mpz_cmp_ui(S2X,0) > 0 && mpz_cmp_ui(S4X,0) > 0)//111
        {
            Q_state = 7;
        }
        else if (mpz_cmp_ui(S0X, 0) > 0 && mpz_cmp_ui(S2X,0) > 0 && mpz_cmp_ui(S4X,0) == 0)//110
        {
            Q_state = 6;
        }
        else if (mpz_cmp_ui(S0X, 0) > 0 && mpz_cmp_ui(S2X,0) == 0 && mpz_cmp_ui(S4X,0) > 0)//101
        {
            Q_state = 5;
        }
        else if (mpz_cmp_ui(S0X, 0) > 0 && mpz_cmp_ui(S2X,0) == 0 && mpz_cmp_ui(S4X,0) == 0)//100
        {
            Q_state = 4;
        }
        else if (mpz_cmp_ui(S0X, 0) == 0 && mpz_cmp_ui(S2X,0) > 0 && mpz_cmp_ui(S4X,0) > 0)//011
        {
            Q_state = 3;
        }
        else if (mpz_cmp_ui(S0X, 0) == 0 && mpz_cmp_ui(S2X,0) > 0 && mpz_cmp_ui(S4X,0) == 0)//010
        {
            Q_state = 2;
        }
        else if (mpz_cmp_ui(S0X, 0) == 0 && mpz_cmp_ui(S2X,0) == 0 && mpz_cmp_ui(S4X,0) > 0)//001
        {
            Q_state = 1;
        }
        else
        {
            Q_state = 0;
        }
        
        switch (Q_state) {
            case 7:
                EFp18_ECA(&tmp_ans, &tmp_ans, &QQ1Q2);
                count_eca_new++;
                break;
            case 6:
                EFp18_ECA(&tmp_ans, &tmp_ans, &QQ1);
                count_eca_new++;
                break;
            case 5:
                EFp18_ECA(&tmp_ans, &tmp_ans, &QQ2);
                count_eca_new++;
                break;
            case 4:
                EFp18_ECA(&tmp_ans, &tmp_ans, P);
                count_eca_new++;
                break;
            case 3:
                EFp18_ECA(&tmp_ans, &tmp_ans, &Q1Q2);
                count_eca_new++;
                break;
            case 2:
                EFp18_ECA(&tmp_ans, &tmp_ans, &Q1);
                count_eca_new++;
                break;
            case 1:
                EFp18_ECA(&tmp_ans, &tmp_ans, &Q2);
                count_eca_new++;
                break;
            case 0:
                break;
        }
        mpz_tdiv_q_2exp(x1,x1,1);
        flag_1st_ECD = 0;
//        gmp_printf("X1= %Zd , case Q %d, Z state = %d\n",x1,Q_state,Z_state);
    }
    
    EFp18_set(ANS,&tmp_ans);
    EFp18_clear(&tmp_ans);
    return;
}

void EFp3_SCM_New(struct EFp3 *ANS, struct EFp3 *P, mpz_t S){
    
    int Z_state,Q_state;
    struct EFp3 tmp_ans;
    EFp3_init(&tmp_ans);
    EFp3_set_infity(&tmp_ans);
    
    //    char x1_binary[(int)mpz_sizeinbase(x1,2)];
    //    mpz_get_str(x1_binary,2,x1);
    //    printf("x1 binary %s\n",x1_binary);
    
    int flag_1st_ECD = 1;
    
    while (mpz_cmp_ui(x1, 0) > 0)
    {
        mpz_and(S1X,x1,s1);
        mpz_and(S3X,x1,s3);
        mpz_and(S5X,x1,s5);
        
        if (!flag_1st_ECD){
            EFp3_ECD(&tmp_ans, &tmp_ans);
            count_ecd_new++;
        }
        
        
        if (mpz_cmp_ui(S1X, 0) > 0 && mpz_cmp_ui(S3X,0) > 0 && mpz_cmp_ui(S5X,0) > 0)//111
        {
            Z_state = 7;
        }
        else if (mpz_cmp_ui(S1X, 0) > 0 && mpz_cmp_ui(S3X,0) > 0 && mpz_cmp_ui(S5X,0) == 0)//110
        {
            Z_state = 6;
        }
        else if (mpz_cmp_ui(S1X, 0) > 0 && mpz_cmp_ui(S3X,0) == 0 && mpz_cmp_ui(S5X,0) > 0)//101
        {
            Z_state = 5;
        }
        else if (mpz_cmp_ui(S1X, 0) > 0 && mpz_cmp_ui(S3X,0) == 0 && mpz_cmp_ui(S5X,0) == 0)//100
        {
            Z_state = 4;
        }
        else if (mpz_cmp_ui(S1X, 0) == 0 && mpz_cmp_ui(S3X,0) > 0 && mpz_cmp_ui(S5X,0) > 0)//011
        {
            Z_state = 3;
        }
        else if (mpz_cmp_ui(S1X, 0) == 0 && mpz_cmp_ui(S3X,0) > 0 && mpz_cmp_ui(S5X,0) == 0)//010
        {
            Z_state = 2;
        }
        else if (mpz_cmp_ui(S1X, 0) == 0 && mpz_cmp_ui(S3X,0) == 0 && mpz_cmp_ui(S5X,0) > 0)//001
        {
            Z_state = 1;
        }
        else
        {
            Z_state = 0;
        }
        
        
        switch (Z_state) {
            case 7:
                EFp3_ECA(&tmp_ans, &tmp_ans, &ZQZQ1ZQ2_d);
                count_eca_new++;
                break;
            case 6:
                EFp3_ECA(&tmp_ans, &tmp_ans, &ZQZQ1_d);
                count_eca_new++;
                break;
            case 5:
                EFp3_ECA(&tmp_ans, &tmp_ans, &ZQZQ2_d);
                count_eca_new++;
                break;
            case 4:
                EFp3_ECA(&tmp_ans, &tmp_ans, &ZQ_d);
                count_eca_new++;
                break;
            case 3:
                EFp3_ECA(&tmp_ans, &tmp_ans, &ZQ1ZQ2_d);
                count_eca_new++;
                break;
            case 2:
                EFp3_ECA(&tmp_ans, &tmp_ans, &ZQ1_d);
                count_eca_new++;
                break;
            case 1:
                EFp3_ECA(&tmp_ans, &tmp_ans, &ZQ2_d);
                count_eca_new++;
                break;
            case 0:
                break;
        }
        
        mpz_and(S0X,x1,s0);
        mpz_and(S2X,x1,s2);
        mpz_and(S4X,x1,s4);
        
        
        if (mpz_cmp_ui(S0X, 0) > 0 && mpz_cmp_ui(S2X,0) > 0 && mpz_cmp_ui(S4X,0) > 0)//111
        {
            Q_state = 7;
        }
        else if (mpz_cmp_ui(S0X, 0) > 0 && mpz_cmp_ui(S2X,0) > 0 && mpz_cmp_ui(S4X,0) == 0)//110
        {
            Q_state = 6;
        }
        else if (mpz_cmp_ui(S0X, 0) > 0 && mpz_cmp_ui(S2X,0) == 0 && mpz_cmp_ui(S4X,0) > 0)//101
        {
            Q_state = 5;
        }
        else if (mpz_cmp_ui(S0X, 0) > 0 && mpz_cmp_ui(S2X,0) == 0 && mpz_cmp_ui(S4X,0) == 0)//100
        {
            Q_state = 4;
        }
        else if (mpz_cmp_ui(S0X, 0) == 0 && mpz_cmp_ui(S2X,0) > 0 && mpz_cmp_ui(S4X,0) > 0)//011
        {
            Q_state = 3;
        }
        else if (mpz_cmp_ui(S0X, 0) == 0 && mpz_cmp_ui(S2X,0) > 0 && mpz_cmp_ui(S4X,0) == 0)//010
        {
            Q_state = 2;
        }
        else if (mpz_cmp_ui(S0X, 0) == 0 && mpz_cmp_ui(S2X,0) == 0 && mpz_cmp_ui(S4X,0) > 0)//001
        {
            Q_state = 1;
        }
        else
        {
            Q_state = 0;
        }
        
        switch (Q_state) {
            case 7:
                EFp3_ECA(&tmp_ans, &tmp_ans, &QQ1Q2_d);
                count_eca_new++;
                break;
            case 6:
                EFp3_ECA(&tmp_ans, &tmp_ans, &QQ1_d);
                count_eca_new++;
                break;
            case 5:
                EFp3_ECA(&tmp_ans, &tmp_ans, &QQ2_d);
                count_eca_new++;
                break;
            case 4:
                EFp3_ECA(&tmp_ans, &tmp_ans, P);
                count_eca_new++;
                break;
            case 3:
                EFp3_ECA(&tmp_ans, &tmp_ans, &Q1Q2_d);
                count_eca_new++;
                break;
            case 2:
                EFp3_ECA(&tmp_ans, &tmp_ans, &Q1_d);
                count_eca_new++;
                break;
            case 1:
                EFp3_ECA(&tmp_ans, &tmp_ans, &Q2_d);
                count_eca_new++;
                break;
            case 0:
                break;
        }
        mpz_tdiv_q_2exp(x1,x1,1);
        flag_1st_ECD = 0;
        //        gmp_printf("X1= %Zd , case Q %d, Z state = %d\n",x1,Q_state,Z_state);
    }
    
    EFp3_set(ANS,&tmp_ans);
    EFp3_clear(&tmp_ans);
    return;
}

void EFp18_to_EFp3_map(struct EFp3 *ANS,struct EFp18 *A){
    
    Fp3_set_ui(&ANS->x,0);
    Fp3_set_ui(&ANS->y,0);
    Fp3_set(&ANS->x,&A->x.x1.x1);
    Fp3_set(&ANS->y,&A->y.x0.x1);
    Fp3_mul_xi(&ANS->x, &ANS->x);
    Fp3_mul_xi(&ANS->y, &ANS->y);
    ANS->infity=A->infity;
}

void EFp3_to_EFp18_map(struct EFp18 *ANS,struct EFp3 *A){
    Fp18_set_ui(&ANS->x,0);
    Fp18_set_ui(&ANS->y,0);
    Fp3_set(&ANS->x.x1.x1,&A->x);
    Fp3_set(&ANS->y.x0.x1,&A->y);
    Fp3_mul(&ANS->x.x1.x1, &ANS->x.x1.x1, &i_inv);
    Fp3_mul(&ANS->y.x0.x1, &ANS->y.x0.x1, &i_inv);
    ANS->infity=A->infity;
}

void pre_calc()
{
    Fp3_init(&i);
    Fp3_set_ui(&i, 0);
    Fp_set_ui(&i.x1, 1);
    //    printf("i = \n");
    //    Fp3_printf(&i);
    
    Fp3_init(&i_inv);
    Fp3_invert(&i_inv, &i);
    //    printf("i inv = \n");
    //    Fp3_printf(&i_inv);
    
    struct Fp3 A;
    Fp3_init(&A);
    Fp3_mul(&A, &i_inv, &i);
    //    printf("i inv *i  = \n");
    //    Fp3_printf(&A);
    
    
    Fp3_init(&i_precom);
    //    mpz_t p_1;
    //    mpz_init(p_1);
    //    mpz_sub_ui(p_1,prime,1);
    //
    //    Fp3_pow(&i_precom, &i_inv, p_1);
    //
    Fp3_pow(&i_precom, &i_inv, prime);
    Fp3_mul_xi(&i_precom, &i_precom);
    //    printf("i precom = \n");
    //    Fp3_printf(&i_precom);
    
    Fp_init(&v1_pre_com);
    Fp_init(&w1_pre_com);
    
    mpz_t p_1, base,tmp;
    mpz_init(p_1);
    mpz_init(base);
    mpz_init(tmp);
    mpz_set_ui(base,2);
    
    mpz_add_ui(p_1,prime,11);
    mpz_tdiv_q_ui(p_1,p_1,18);
    mpz_powm(w1_pre_com.x0,base,p_1,prime);
    
    mpz_sub_ui(p_1,prime,1);
    mpz_tdiv_q_ui(p_1,p_1,6);
    mpz_powm(v1_pre_com.x0,base,p_1,prime);
    
    mpz_mul_ui(tmp,prime,2);
    mpz_sub_ui(tmp,tmp,5);
    mpz_tdiv_q_ui(tmp,tmp,9);
    mpz_powm(tmp,base,tmp,prime);
    mpz_set(w1_pre_com.x0,tmp);
    
    //    if (mpz_divisible_ui_p(tmp,9) > 0)
    //    {
    //        printf("Yes=====");
    //    }
    //    else
    //    {
    //        printf("Not =====");
    //    }
    
    mpz_clear(p_1);
    mpz_clear(base);
    mpz_clear(tmp);
}

void Skew_Frobenius_map(struct EFp3 *ANS, struct EFp3 *Qt)
{
    struct EFp3 tmp_ans;
    EFp3_init(&tmp_ans);
    
    struct Fp3 Qt_x, Qt_y;
    Fp3_init(&Qt_x);
    Fp3_init(&Qt_y);
    Fp3_set(&Qt_x, &Qt->x);
    Fp3_set(&Qt_y, &Qt->y);
    
    
    Fp3_pow(&Qt_x, &Qt_x, prime);
    Fp3_mul(&Qt_x, &Qt_x, &i_precom);
    Fp3_mul_Fp(&tmp_ans.x, &Qt_x, &w1_pre_com);
    Fp3_mul_xi(&tmp_ans.x, &tmp_ans.x);
    
    Fp3_pow(&Qt_y, &Qt_y, prime);
    Fp3_mul(&tmp_ans.y, &Qt_y, &i_precom);
    Fp3_mul_Fp(&tmp_ans.y, &tmp_ans.y, &v1_pre_com);
    
    EFp3_set(ANS,&tmp_ans);
    EFp3_clear(&tmp_ans);
    Fp3_clear(&Qt_x);
    Fp3_clear(&Qt_y);
}

void get_precalc_skew_frobenious_mapped_points_in_EFp3(struct EFp3 *ANS){
    struct EFp3 tmp,tmp1;
    EFp3_init(&tmp);
    EFp3_init(&tmp1);
    
    EFp3_init(&Q1_d);
    EFp3_init(&Q2_d);
    EFp3_init(&ZQ_d);
    EFp3_init(&ZQ1_d);
    EFp3_init(&ZQ2_d);
    EFp3_init(&QQ1_d);
    EFp3_init(&QQ2_d);
    EFp3_init(&Q1Q2_d);
    EFp3_init(&QQ1Q2_d);
    EFp3_init(&ZQZQ1_d);
    EFp3_init(&ZQZQ2_d);
    EFp3_init(&ZQ1ZQ2_d);
    EFp3_init(&ZQZQ1ZQ2_d);
    
    
    struct EFp3 Q_twisted;
    EFp3_init(&Q_twisted);
    
    EFp18_to_EFp3_map(&Q_twisted, &QinG2);                 //QinG2=Q=Q'
    Skew_Frobenius_map(&Q2_d, &Q_twisted);                //Q2_d=phi'(Q')
    
    mpz_t sclr_tmp;
    mpz_init(sclr_tmp);
    mpz_set_ui(sclr_tmp,8);
    EFp3_SCM_BIN(&tmp, &Q_twisted, sclr_tmp);//8Q'
    Skew_Frobenius_map(&tmp1, &tmp);               //phi(8Q')
    Skew_Frobenius_map(&Q1_d, &tmp1);               //phi^2(8Q)
    
    mpz_set_ui(sclr_tmp,5);
    EFp3_SCM_BIN(&tmp, &Q_twisted, sclr_tmp);                 //5Q
    Skew_Frobenius_map(&tmp1, &tmp);               //phi(5Q)
    Skew_Frobenius_map(&tmp, &tmp1);
    Skew_Frobenius_map(&tmp1, &tmp);
    Skew_Frobenius_map(&tmp, &tmp1);
    Skew_Frobenius_map(&tmp1, &tmp);
    Fp3_neg(&tmp1.y,&tmp1.y);
    EFp3_ECA(&Q1_d, &Q1_d, &tmp1);                     //Q1=phi^2(8Q)-phi^5(5Q)
    count_eca_new_pre++;
    
    EFp3_ECA(&QQ1_d,&Q_twisted,&Q1_d);
    EFp3_ECA(&QQ2_d,&Q_twisted,&Q2_d);
    EFp3_ECA(&Q1Q2_d,&Q1_d,&Q2_d);
    EFp3_ECA(&QQ1Q2_d,&Q_twisted,&Q1Q2_d);
    count_eca_new_pre = count_eca_new_pre + 4;
    
    mpz_set_ui(sclr_tmp,3);                         //ZQ
    EFp3_SCM_BIN(&tmp, &Q_twisted, sclr_tmp);
    Fp3_neg(&tmp.y,&tmp.y);
    Skew_Frobenius_map(&ZQ_d, &tmp);
    
    Skew_Frobenius_map(&tmp, &Q_twisted);
    Skew_Frobenius_map(&tmp1, &tmp);
    Skew_Frobenius_map(&tmp, &tmp1);
    Skew_Frobenius_map(&tmp1, &tmp);
    EFp3_ECA(&ZQ_d, &ZQ_d, &tmp1);                     //ZQ=phi(-3Q)+phi^4(Q)
    count_eca_new_pre = count_eca_new_pre + 1;
    
    EFp3_SCM_BIN(&tmp, &Q1_d, sclr_tmp);                 //ZQ1
    Fp3_neg(&tmp.y,&tmp.y);
    Skew_Frobenius_map(&ZQ1_d, &tmp);
    
    Skew_Frobenius_map(&tmp, &Q1_d);
    Skew_Frobenius_map(&tmp1, &tmp);
    Skew_Frobenius_map(&tmp, &tmp1);
    Skew_Frobenius_map(&tmp1, &tmp);
    EFp3_ECA(&ZQ1_d, &ZQ1_d, &tmp1);                     //ZQ1=phi(-3Q1)+phi^4(Q1)
    count_eca_new_pre = count_eca_new_pre + 1;
    
    EFp3_SCM_BIN(&tmp, &Q2_d, sclr_tmp);                 //ZQ2
    Fp3_neg(&tmp.y,&tmp.y);
    Skew_Frobenius_map(&ZQ2_d, &tmp);
    
    Skew_Frobenius_map(&tmp, &Q2_d);
    Skew_Frobenius_map(&tmp1, &tmp);
    Skew_Frobenius_map(&tmp, &tmp1);
    Skew_Frobenius_map(&tmp1, &tmp);
    EFp3_ECA(&ZQ2_d, &ZQ2_d, &tmp1);                     //ZQ2=phi(-3Q2)+phi^4(Q2)
    count_eca_new_pre = count_eca_new_pre + 1;
    
    EFp3_ECA(&ZQZQ1_d,&ZQ_d,&ZQ1_d);
    EFp3_ECA(&ZQZQ2_d,&ZQ_d,&ZQ2_d);
    EFp3_ECA(&ZQ1ZQ2_d,&ZQ1_d,&ZQ2_d);
    EFp3_ECA(&ZQZQ1ZQ2_d,&ZQ_d,&ZQ1ZQ2_d);
    count_eca_new_pre = count_eca_new_pre + 4;
    
    EFp3_set(ANS,&Q_twisted);
    EFp3_clear(&tmp);
    EFp3_clear(&tmp1);
    mpz_clear(sclr_tmp);
}

void Check_SCM(struct EFp18 *ANS) {
    
    struct EFp3 ans_temp_EFp3, twisted_Q;
    EFp3_init(&ans_temp_EFp3);
    EFp3_init(&twisted_Q);
    
    struct EFp18 temp_EFp18;
    EFp18_init(&temp_EFp18);
    
    EFp18_random_set_for_Ate(&QinG2);
    count_eca_new_pre=0; count_ecd_new_pre=0;
    get_precalc_frobenious_mapped_points_in_EFp18(&G2Q);
    
    count_eca_new_pre=0; count_ecd_new_pre=0;
    get_precalc_skew_frobenious_mapped_points_in_EFp3(&twisted_Q);
    
    //===================== SM test============
    mpz_t s;
    mpz_init(s);
//    mpz_set_str(s,"17550985122267692163301219011",10);
    
    
    FILE *fp;
//    fp=fopen("Experiment_ICISC_SFM_OS_with_mapping_02.csv","a");
    fp=fopen("Experiment_SCIS_11.csv","a");
    
    if(fp == NULL){
        printf("Couldn't open file\n");
    }
//    gmp_fprintf(fp,"S,HW,s_size,NEW_#ECA,NEW_#ECD,BIN_#ECA,BIN_#ECD,WIN_ECA,WIN_ECD,NAF_ECA,NAF_ECD,NEW_time,BIN_time,WIN_time,NAF_time\n");
    
    gmp_fprintf(fp,"S,HW,s_size,NEW_ECA,NEW_ECD,BIN_ECA,BIN_ECD,BIN_ECA_MAP,BIN_ECD_MAP,WIN_ECA,WIN_ECD,WIN_ECA_MAP,WIN_ECD_MAP,NAF_ECA,NAF_ECD,NAF_ECA_MAP,NAF_ECD_MAP,ML_ECA,ML_ECD,ML_ECA_MAP,ML_ECD_MAP,NEW_time,BIN_time,BIN_time_map,WIN_time,WIN_time_map,NAF_time,NAF_time_map,ML_time,ML_time_map\n");
    
    
    int k = 0;
    for (k = 0; k < 2; k++) {
        
//        mpz_random(s,10);
//        mpz_mod(s,s,order_r);
        mpz_set(s,order_r);
        int length = (int)mpz_sizeinbase(s,2);
        
        get_z_adic(s);
        
        struct timeval t0;
        struct timeval t1;
        float elapsed_new;
        float elapsed_BIN;
        float elapsed_BIN_map;
        float elapsed_WIN;
        float elapsed_WIN_map;
        float elapsed_NAF;
        float elapsed_NAF_map;
        float elapsed_ML;
        float elapsed_ML_map;
        
        
        printf("Proposed approach\n");
        count_eca_new =0; count_ecd_new=0;
        
        gettimeofday(&t0, 0);
//        EFp18_SCM_New(&temp_EFp18, &QinG2, s);
        EFp3_SCM_New(&ans_temp_EFp3,&twisted_Q,s);
        gettimeofday(&t1, 0);
        EFp3_to_EFp18_map(&temp_EFp18, &ans_temp_EFp3);
        elapsed_new = timedifference_msec(t0, t1);
        printf("ECA = %d, ECD = %d, ECA_PRE = %d, ECD_PRE = %d, Elapsed Time = %.2f\n",count_eca_new,count_ecd_new,count_eca_new_pre,count_ecd_new_pre, elapsed_new );
        EFp18_printf(&temp_EFp18);

        
        printf("\n Binary method with mapping\n");
        gettimeofday(&t0, 0);
        EFp3_SCM_BIN(&ans_temp_EFp3, &twisted_Q, s);
        gettimeofday(&t1, 0);
        EFp3_to_EFp18_map(&temp_EFp18, &ans_temp_EFp3);
        elapsed_BIN_map = timedifference_msec(t0, t1);
        printf("Old count ECA = %d, ECD = %d, Elasped old = %.2f\n",count_eca_BIN_map,count_ecd_BIN_map,elapsed_BIN_map);
        EFp18_printf(&temp_EFp18);
        
        printf("\n BIN method mapping\n");
        gettimeofday(&t0, 0);
        EFp18_SCM_BIN(&temp_EFp18, &QinG2, s);
        gettimeofday(&t1, 0);
        elapsed_BIN = timedifference_msec(t0, t1);
        printf("Count ECA = %d, ECD = %d, Elasped old = %.2f\n",count_eca_BIN,count_ecd_BIN,elapsed_BIN);
        EFp18_printf(&temp_EFp18);
        
        printf("\n  Window method map\n");
        gettimeofday(&t0, 0);
        EFp3_SCM_WIN(&ans_temp_EFp3, &twisted_Q, s);
        gettimeofday(&t1, 0);
        elapsed_WIN_map = timedifference_msec(t0, t1);
        printf("Old count ECA = %d, ECD = %d, Elasped old = %.2f\n",count_eca_WIN_map,count_ecd_WIN_map,elapsed_WIN_map);
        EFp18_printf(&temp_EFp18);
        
        printf("\n WIN method without mapping\n");
        gettimeofday(&t0, 0);
        EFp18_SCM_WIN(&temp_EFp18, &QinG2, s);
        gettimeofday(&t1, 0);
        elapsed_WIN = timedifference_msec(t0, t1);
        printf("Count ECA = %d, ECD = %d, Elasped old = %.2f\n",count_eca_WIN,count_ecd_WIN,elapsed_WIN);
        EFp18_printf(&temp_EFp18);
        
        
        printf("\n EFp18_SCM_BIN :: NAF method\n");
        gettimeofday(&t0, 0);
        EFp3_SCM_NAF(&ans_temp_EFp3, &twisted_Q, s);
        gettimeofday(&t1, 0);
        elapsed_NAF_map = timedifference_msec(t0, t1);
        printf("Old count ECA = %d, ECD = %d, Elasped old = %.2f\n",count_eca_NAF_map,count_ecd_NAF_map,elapsed_NAF_map);
        EFp18_printf(&temp_EFp18);
        
        printf("\n NAF method wihtout mapping\n");
        gettimeofday(&t0, 0);
        EFp18_SCM_NAF(&temp_EFp18, &QinG2, s);
        gettimeofday(&t1, 0);
        elapsed_NAF = timedifference_msec(t0, t1);
        printf("Count ECA = %d, ECD = %d, Elasped old = %.2f\n",count_eca_NAF,count_ecd_NAF,elapsed_NAF);
        EFp18_printf(&temp_EFp18);
        
        printf("\n ML method wihtout mapping\n");
        gettimeofday(&t0, 0);
        EFp18_SCM_ML(&temp_EFp18, &QinG2, s);
        gettimeofday(&t1, 0);
        elapsed_ML = timedifference_msec(t0, t1);
        printf("Count ECA = %d, ECD = %d, Elasped old = %.2f\n",count_eca_ML,count_ecd_ML,elapsed_ML);
        EFp18_printf(&temp_EFp18);
        
        
        printf("\n ML method with mapping\n");
        gettimeofday(&t0, 0);
        EFp3_SCM_ML(&ans_temp_EFp3, &twisted_Q, s);
        gettimeofday(&t1, 0);
        EFp3_to_EFp18_map(&temp_EFp18, &ans_temp_EFp3);
        elapsed_ML_map = timedifference_msec(t0, t1);
        printf("Count ECA = %d, ECD = %d, Elasped old = %.2f\n",count_eca_ML_map,count_ecd_ML_map,elapsed_ML_map);
        EFp18_printf(&temp_EFp18);

        
        if(fp == NULL){
            printf("Couldn't open file\n");
        }
        int r_p = count_eca_new + count_eca_new_pre;
//        gmp_fprintf(fp,"%Zd,%ld,%d,%d,%d,%d,%d,%d,%d,%d,%d,%f,%f,%f,%f\n",s, mpz_popcount(s),length,r_p, count_ecd_new, count_eca_BIN, count_ecd_BIN,count_eca_WIN,count_ecd_WIN,count_eca_NAF,count_ecd_NAF, elapsed_new,elapsed_BIN,elapsed_WIN,elapsed_NAF);
        
        gmp_fprintf(fp,"%Zd,%ld,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%f,%f,%f,%f,%f,%f,%f,%f,%f\n\n\n",s, mpz_popcount(s),length,r_p, count_ecd_new, count_eca_BIN, count_ecd_BIN,count_eca_BIN_map,count_ecd_BIN_map,count_eca_WIN,count_ecd_WIN,count_eca_WIN_map,count_ecd_WIN_map,count_eca_NAF,count_ecd_NAF,count_eca_NAF_map,count_ecd_NAF_map,count_eca_ML,count_ecd_ML,count_eca_ML_map,count_ecd_ML_map,elapsed_new,elapsed_BIN,elapsed_BIN_map,elapsed_WIN,elapsed_WIN_map,elapsed_NAF,elapsed_NAF_map,elapsed_ML,elapsed_ML_map);
    }
    fclose(fp);
    EFp3_clear(&ans_temp_EFp3);
    EFp3_clear(&twisted_Q);
    EFp18_clear(&temp_EFp18);
    mpz_clear(s);
    
}


void EFp18_set_EFp(struct EFp18 *A,struct EFp *B){
    Fp18_set_ui(&A->x,0);
    Fp18_set_ui(&A->y,0);
    
    Fp_set(&A->x.x0.x0.x0,&B->x);
    Fp_set(&A->y.x0.x0.x0,&B->y);
    A->infity=B->infity;
}
void EFp18_frobenius_map(struct EFp18 *ANS,struct EFp18 *A){
    struct EFp18 tmp;
    EFp18_init(&tmp);
    
    Fp18_frobenius_map(&tmp.x,&A->x);
    Fp18_frobenius_map(&tmp.y,&A->y);
    
    EFp18_set(ANS,&tmp);
    
    EFp18_clear(&tmp);
}

void EFp18_random_set_for_Ate(struct EFp18 *ANS){
    struct EFp18 P,P_frobenius,tmp_EFp18;
    EFp18_init(&P);
    EFp18_init(&P_frobenius);
    EFp18_init(&tmp_EFp18);
    
    EFp18_random_set(&P);
    
    EFp18_frobenius_map(&P_frobenius,&P);
    Fp18_neg(&tmp_EFp18.y,&P.y);
    Fp18_set(&tmp_EFp18.x,&P.x);
    
    EFp18_ECA(&tmp_EFp18,&tmp_EFp18,&P_frobenius);
    
    EFp18_set(ANS,&tmp_EFp18);
    
    EFp18_clear(&P);
    EFp18_clear(&P_frobenius);
    EFp18_clear(&tmp_EFp18);
}

//-----------------------------------------------------------------------------------------
#pragma mark Parameters methods

float timedifference_msec(struct timeval t0, struct timeval t1)
{
    return (t1.tv_sec - t0.tv_sec) * 1000.0f + (t1.tv_usec - t0.tv_usec) / 1000.0f;
}

void EFp_set_EC_parameter(void){
    //set p,r
    mpz_t p_tmp,r_tmp,t_tmp;
    mpz_t xpow2,xpow3,xpow4,xpow5,xpow6,xpow7,xpow8;
    mpz_t tmp1,tmp2;
    
    mpz_init(p_tmp);
    mpz_init(r_tmp);
    mpz_init(t_tmp);
    
    mpz_init(xpow2);
    mpz_init(xpow3);
    mpz_init(xpow4);
    mpz_init(xpow5);
    mpz_init(xpow6);
    mpz_init(xpow7);
    mpz_init(xpow8);
    
    mpz_init(tmp1);
    mpz_init(tmp2);
    
    mpz_mul(xpow2,X,X);
    mpz_mul(xpow3,xpow2,X);
    mpz_mul(xpow4,xpow2,xpow2);
    mpz_mul(xpow5,xpow4,X);
    mpz_mul(xpow6,xpow3,xpow3);
    mpz_mul(xpow7,xpow6,X);
    mpz_mul(xpow8,xpow4,xpow4);
    
    //t=1/7(x^4+16x+7)
    mpz_mul_ui(tmp1,X,16);
    mpz_add_ui(tmp2,xpow4,7);
    mpz_add(t_tmp,tmp1,tmp2);
    
    mpz_div_ui(trace_t,t_tmp,7);
    
    //r=x^6+37x^3+343/343
    mpz_mul_ui(tmp1,xpow3,37);
    mpz_add_ui(tmp2,xpow6,343);
    mpz_add(r_tmp,tmp1,tmp2);
    
    mpz_div_ui(order_r,r_tmp,343);
    
    // mpz_set(r,r_tmp);
    
    //p=1/21(x^8+5x^7+7x^6+37x^5+188x^4+259x^3+343x^2+1763x+2401)
    mpz_mul_ui(tmp1,xpow7,5);
    mpz_add(p_tmp,tmp1,xpow8);
    mpz_mul_ui(tmp1,xpow6,7);
    mpz_add(p_tmp,tmp1,p_tmp);
    mpz_mul_ui(tmp1,xpow5,37);
    mpz_add(p_tmp,tmp1,p_tmp);
    mpz_mul_ui(tmp1,xpow4,188);
    mpz_add(p_tmp,tmp1,p_tmp);
    mpz_mul_ui(tmp1,xpow3,259);
    mpz_add(p_tmp,tmp1,p_tmp);
    mpz_mul_ui(tmp1,xpow2,343);
    mpz_add(p_tmp,tmp1,p_tmp);
    mpz_mul_ui(tmp1,X,1763);
    mpz_add(p_tmp,tmp1,p_tmp);
    mpz_add_ui(p_tmp,p_tmp,2401);
    
    mpz_div_ui(prime,p_tmp,21);
    // mpz_set(p,p_tmp);
    
    mpz_add_ui(order_EFp,prime,1);
    mpz_sub(order_EFp,order_EFp,trace_t);
    
    if(mpz_probab_prime_p(prime,25)==0){
        gmp_printf("p:%Zd\n",prime);
        printf("not  prime number!\n");
        exit(0);
    }
    
    struct EFp P,ANS;
    int legendle;
    struct Fp a,x;
    mpz_t tmp_b;
    Fp_init(&a);
    EFp_init(&P);
    EFp_init(&ANS);
    Fp_init(&x);
    mpz_init(tmp_b);
    mpz_set_ui(tmp_b,0);
    
    for(;;){
        mpz_add_ui(tmp_b,tmp_b,1);
        Fp_set_ui(&x,1);
        legendle=0;
        while(legendle!=1){
            mpz_powm_ui(a.x0,x.x0,3,prime);
            mpz_add(a.x0,a.x0,tmp_b);
            if((legendle=mpz_legendre(a.x0,prime))==1){
                Fp_sqrt(&P.y,&a);
                Fp_set(&P.x,&x);
                EFp_SCM_BIN(&ANS,&P,order_EFp);
                if(ANS.infity==TRUE){
                    mpz_set(b,tmp_b);
                    mpz_clear(tmp_b);
                    Fp_clear(&a);
                    Fp_clear(&x);
                    EFp_clear(&P);
                    EFp_clear(&ANS);
                    return;
                }
            }
            Fp_add_ui(&x,&x,1);
        }
    }
    return;
}

//-----------------------------------------------------------------------------------------
#pragma mark Pairing methods
void Miller_algo(struct Fp18 *ANS,struct EFp18 *P,struct EFp18 *Q, mpz_t roop){
    struct Fp18 f,l_sum,v_sum;
    Fp18_init(&f);
    Fp18_init(&l_sum);
    Fp18_init(&v_sum);
    Fp_set_ui(&l_sum.x0.x0.x0,1);
    Fp_set_ui(&v_sum.x0.x0.x0,1);
    
    struct EFp18 T;
    EFp18_init(&T);
    EFp18_set(&T,P);
    
    
    struct Fp18 ltt,ltp,v2t,vtp;
    Fp18_init(&ltt);
    Fp18_init(&ltp);
    Fp18_init(&v2t);
    Fp18_init(&vtp);
    
    int i;
    struct Fp18 tmp1,lambda;
    Fp18_init(&tmp1);
    Fp18_init(&lambda);
    
    int r_bit;//bit数
    r_bit= (int)mpz_sizeinbase(roop,2);
    
    for(i=r_bit-2;i>=0;i--){
        Fp18_mul(&l_sum,&l_sum,&l_sum);
        Fp18_mul(&v_sum,&v_sum,&v_sum);
        
        ltt_q(&ltt,&T,Q);
        Fp18_mul(&l_sum,&l_sum,&ltt);
        
        EFp18_ECD(&T,&T);
        
        v2t_q(&v2t,&T,Q);
        Fp18_mul(&v_sum,&v_sum,&v2t);
        Fp18_mul(&f,&f,&tmp1);
        
        if(mpz_tstbit(roop,i)==1){
            // ltp_q(&ltp,&T,P,Q);
            ltp_q(&ltp,&T,Q,P);
            Fp18_mul(&l_sum,&l_sum,&ltp);
            
            EFp18_ECA(&T,&T,P);
            
            vtp_q(&vtp,&T,Q);
            Fp18_mul(&v_sum,&v_sum,&vtp);
        }
//        else{
//  
//        }
    }
    
    Fp18_div(&f,&l_sum,&v_sum);
    // printf("v=");
    // Fp18_printf(&v_sum);
    // Fp18_set(ANS,&f);
    Fp18_set(ANS,&l_sum);
    
    Fp18_clear(&f);
    EFp18_clear(&T);
    Fp18_clear(&tmp1);
    Fp18_clear(&lambda);
}
void Optimal_Miller_algo(struct Fp18 *ANS,struct EFp18 *P,struct EFp18 *Q,mpz_t roop){
    struct Fp18 f,l_sum,v_sum;
    Fp18_init(&f);
    Fp18_init(&l_sum);
    Fp18_init(&v_sum);
    Fp_set_ui(&l_sum.x0.x0.x0,1);
    Fp_set_ui(&v_sum.x0.x0.x0,1);
    
    struct EFp18 T,R;
    EFp18_init(&T);
    EFp18_init(&R);
    EFp18_set(&T,P);
    
    mpz_t p3;
    mpz_init(p3);
    
    struct Fp18 ltt,ltp,v2t,vtp;
    Fp18_init(&ltt);
    Fp18_init(&ltp);
    Fp18_init(&v2t);
    Fp18_init(&vtp);
    
    int i;
    struct Fp18 tmp1,lambda;
    Fp18_init(&tmp1);
    Fp18_init(&lambda);
    
    int r_bit;//bit数
    r_bit= (int)mpz_sizeinbase(roop,2);
    
    for(i=r_bit-2;i>=0;i--){
        if(mpz_tstbit(roop,i)==1){
            Fp18_mul(&l_sum,&l_sum,&l_sum);
            Fp18_mul(&v_sum,&v_sum,&v_sum);
            
            ltt_q(&ltt,&T,Q);
            Fp18_mul(&l_sum,&l_sum,&ltt);
            
            EFp18_ECD(&T,&T);
            
            v2t_q(&v2t,&T,Q);
            Fp18_mul(&v_sum,&v_sum,&v2t);
            Fp18_mul(&f,&f,&tmp1);
            
            // ltp_q(&ltp,&T,P,Q);
            ltp_q(&ltp,&T,Q,P);
            Fp18_printf(&ltp);
            Fp18_mul(&l_sum,&l_sum,&ltp);
            
            EFp18_ECA(&T,&T,P);
            
            vtp_q(&vtp,&T,Q);
            Fp18_mul(&v_sum,&v_sum,&vtp);
        }else{
            Fp18_mul(&l_sum,&l_sum,&l_sum);
            Fp18_mul(&v_sum,&v_sum,&v_sum);
            
            ltt_q(&ltt,&T,Q);
            Fp18_mul(&l_sum,&l_sum,&ltt);
            
            EFp18_ECD(&T,&T);
            
            v2t_q(&v2t,&T,Q);
            Fp18_mul(&v_sum,&v_sum,&v2t);
            Fp18_mul(&f,&f,&tmp1);
        }
    }
    
    Fp18_div(&f,&l_sum,&v_sum);
    
    mpz_mul_ui(p3,prime,3);
    EFp18_SCM_BIN(&R,P,p3);
    
    ltp_q(&ltp,&T,&R,Q);
    Fp18_mul(&l_sum,&l_sum,&ltp);
    
    Fp18_set(ANS,&l_sum);
    
    Fp18_clear(&f);
    EFp18_clear(&T);
    Fp18_clear(&tmp1);
    Fp18_clear(&lambda);
}
void Final_Exp(struct Fp18 *ANS,struct Fp18 *A){
    mpz_t p18;
    mpz_init(p18);
    
    mpz_pow_ui(p18,prime,18);
    mpz_sub_ui(p18,p18,1);
    mpz_div(p18,p18,order_r);
    
    Fp18_pow(ANS,A,p18);
    
    mpz_clear(p18);
}
void Tate_Pairing(struct Fp18 *ANS,struct EFp18 *G1,struct EFp18 *G2){
    struct Fp18 t_ans;
    Fp18_init(&t_ans);
    
    Miller_algo(&t_ans,G2,G1,order_EFp);
    Final_Exp(&t_ans,&t_ans);
    Fp18_set(ANS,&t_ans);
    
    Fp18_clear(&t_ans);
}
void Ate_Pairing(struct Fp18 *ANS,struct EFp18 *G1,struct EFp18 *G2){
    struct Fp18 t_ans;
    Fp18_init(&t_ans);
    
    mpz_t tm1;
    mpz_init(tm1);
    mpz_sub_ui(tm1,trace_t,1);
    
    Miller_algo(&t_ans,G2,G1,tm1);
//    Miller_algo(&t_ans,G1,G2,tm1);
    Final_Exp(&t_ans,&t_ans);
    Fp18_set(ANS,&t_ans);
    
    Fp18_clear(&t_ans);
}
void Optimal_Ate_Pairing(struct Fp18 *ANS,struct EFp18 *G1,struct EFp18 *G2){
    mpz_t p3;
    mpz_init(p3);
    struct EFp18 zQ;
    EFp18_init(&zQ);
    
    struct Fp18 ltp;
    Fp18_init(&ltp);
    
    struct Fp18 Miller_X,Miller_3;
    Fp18_init(&Miller_X);
    Fp18_init(&Miller_3);
    
    struct Fp18 t_ans;
    Fp18_init(&t_ans);
    Optimal_Miller_algo(&Miller_X,G2,G1,X);
    
    mpz_t set_3;
    mpz_init(set_3);
    mpz_set_ui(set_3,3);
    
    Miller_algo(&Miller_3,G2,G1,set_3);
    Fp18_pow(&Miller_3,&Miller_3,prime);
    
    Fp18_mul(&t_ans,&Miller_X,&Miller_3);
    
    Final_Exp(ANS,&t_ans);
    
}
void ltt_q(struct Fp18 *ANS,struct EFp18 *T,struct EFp18 *Q){
    struct Fp18 tmp1,tmp2,tmp3,lambda,ltt;
    Fp18_init(&tmp1);
    Fp18_init(&tmp2);
    Fp18_init(&tmp3);
    Fp18_init(&lambda);
    Fp18_init(&ltt);
    
    Fp18_mul(&tmp1,&T->x,&T->x);//xt^2
    Fp18_add(&tmp2,&tmp1,&tmp1);
    Fp18_add(&tmp1,&tmp2,&tmp1);//3xt^3
    Fp18_add(&tmp2,&T->y,&T->y);//2yt
    
    Fp18_div(&lambda,&tmp1,&tmp2);//lambda=3xt^2/2yt
    Fp18_sub(&tmp3,&Q->x,&T->x);//tmp3=xq-xt
    Fp18_mul(&tmp3,&tmp3,&lambda);//tmp3=lambda(xq-xt)
    
    Fp18_sub(&ltt,&Q->y,&T->y);//yq-yt
    Fp18_sub(&ltt,&ltt,&tmp3);//ltt=yq-yt-lambda(xq-xt)
    
    Fp18_set(ANS,&ltt);
    
    Fp18_clear(&tmp1);
    Fp18_clear(&tmp2);
    Fp18_clear(&tmp3);
    Fp18_clear(&lambda);
    Fp18_clear(&ltt);
}
void v2t_q(struct Fp18 *ANS,struct EFp18 *T,struct EFp18 *Q){
    struct Fp18 v2t;
    Fp18_init(&v2t);
    
    Fp18_sub(&v2t,&Q->x,&T->x);//v2t=xq-xt
    Fp18_set(ANS,&v2t);
    
    Fp18_clear(&v2t);
}
void ltp_q(struct Fp18 *ANS,struct EFp18 *T,struct EFp18 *P,struct EFp18 *Q){
    struct Fp18 tmp1,tmp2,tmp3,tmp4,lambda,ltp;
    Fp18_init(&tmp1);
    Fp18_init(&tmp2);
    Fp18_init(&tmp3);
    Fp18_init(&tmp4);
    Fp18_init(&lambda);
    Fp18_init(&ltp);
    
    if((Fp18_cmp(&T->x,&P->x))==0&&(Fp18_cmp(&T->y,&P->y))!=0){//xt==xp&&yt!=yp
        Fp18_sub(&ltp,&Q->x,&T->x);
        Fp18_set(ANS,&ltp);
        
        return;
    }
    
    Fp18_sub(&tmp1,&T->x,&P->x);//xt-xp
    Fp18_sub(&tmp2,&T->y,&P->y);//yt-yp
    Fp18_div(&lambda,&tmp2,&tmp1);//lambda=(yt-tp)/(xt-xp)
    
    Fp18_sub(&tmp3,&Q->x,&T->x);//tmp3=(xq-xt)
    Fp18_mul(&tmp4,&tmp3,&lambda);//tmp4=lambda(xq-xt)
    
    Fp18_sub(&ltp,&Q->y,&T->y);//ltp=yq-yt
    Fp18_sub(&ltp,&ltp,&tmp4);//ltp=yq-yt-lambda(xq-xt)
    
    Fp18_set(ANS,&ltp);
    
    Fp18_clear(&tmp1);
    Fp18_clear(&tmp2);
    Fp18_clear(&tmp3);
    Fp18_clear(&tmp4);
    Fp18_clear(&lambda);
    Fp18_clear(&ltp);
}
void vtp_q(struct Fp18 *ANS,struct EFp18 *T,struct EFp18 *Q){
    struct Fp18 vtp;
    Fp18_init(&vtp);
    if(T->infity==1){//if T is infity
        Fp18_set_ui(ANS,0);
        Fp_set_ui(&ANS->x0.x0.x0,1);
        return;
    }
    
    Fp18_sub(&vtp,&Q->x,&T->x);
    Fp18_set(ANS,&vtp);
    
    Fp18_clear(&vtp);
}
void check_Pairing(void){
    struct EFp tmp;
    EFp_init(&tmp);
    
    struct EFp18 P,Q,R,S;
    EFp18_init(&P);
    EFp18_init(&Q);
    EFp18_init(&R);
    EFp18_init(&S);
    
    struct Fp18 ans,tmp1,tmp2,tmp3;
    Fp18_init(&ans);
    Fp18_init(&tmp1);
    Fp18_init(&tmp2);
    Fp18_init(&tmp3);
    
    mpz_t a,b,ab;
    mpz_init(a);
    mpz_init(b);
    mpz_init(ab);
    
    mpz_set_ui(a,31);
    mpz_set_ui(b,11);
    mpz_mul(ab,a,b);
    //---------------------------------------------------
    // printf("Tate Pairing\n");
    // EFp_random_set(&tmp);
    // EFp18_set_EFp(&P,&tmp);
    //
    // EFp18_random_set(&Q);
    //
    // Tate_Pairing(&tmp1,&P,&Q);
    // printf("G1=");
    // EFp18_printf(&P);
    // printf("G2=");
    // EFp18_printf(&Q);
    //
    // Fp18_pow(&tmp1,&tmp1,ab);
    // printf("f^ab=");
    // Fp18_printf(&tmp1);
    //
    // EFp18_SCM_BIN(&R,&P,a);
    // EFp18_SCM_BIN(&S,&Q,b);
    //
    // Tate_Pairing(&tmp2,&R,&S);
    //
    // printf("f'  =");
    // Fp18_printf(&tmp2);
    //----------------------------------------------------
     printf("Ate Pairing\n");
     EFp_random_set(&tmp);
     EFp18_set_EFp(&P,&tmp);
    
     EFp18_random_set_for_Ate(&Q);
     printf("G1=");
     EFp18_printf(&P);
     printf("G2=");
     EFp18_printf(&Q);
    
     Ate_Pairing(&tmp1,&P,&Q);
    
     Fp18_pow(&tmp1,&tmp1,ab);
     printf("f^ab=");
     Fp18_printf(&tmp1);
    
     EFp18_SCM_BIN(&R,&P,a);
     EFp18_SCM_BIN(&S,&Q,b);
    
     Ate_Pairing(&tmp2,&R,&S);
    
     printf("f'  =");
     Fp18_printf(&tmp2);
    //----------------------------------------------------
//    printf("Optimal Ate Pairing\n");
//    EFp_random_set(&tmp);
//    EFp18_set_EFp(&P,&tmp);
//    
//    EFp18_random_set_for_Ate(&Q);
//    
//    printf("G1=");
//    EFp18_printf(&P);
//    printf("G2=");
//    EFp18_printf(&Q);
//    
//    
//    Optimal_Ate_Pairing(&tmp1,&P,&Q);
//    Fp18_pow(&tmp1,&tmp1,ab);
//    printf("f^ab=");
//    Fp18_printf(&tmp1);
//    
//    EFp18_SCM_BIN(&R,&P,a);
//    EFp18_SCM_BIN(&S,&Q,b);
//    
//    Optimal_Ate_Pairing(&tmp2,&R,&S);
//    
//    printf("f'  =");
//    Fp18_printf(&tmp2);
    //----------------------------------------------------
    
    
    EFp18_clear(&Q);
    Fp18_clear(&ans);
    Fp18_clear(&tmp1);
    Fp18_clear(&tmp2);
    Fp18_clear(&tmp3);
    mpz_clear(a);
    mpz_clear(b);
    mpz_clear(ab);
}


void EFp_SCM_ML(struct EFp *RES, struct EFp *P,mpz_t scalar){
    int i,length;
    length= (int)mpz_sizeinbase(scalar,2);
    char r_binary[length];
    mpz_get_str(r_binary,2,scalar);
    struct EFp T0,T1;
    EFp_init(&T0);
    
    EFp_set_infity(&T0);
    EFp_init(&T1);
    EFp_set(&T1, P);
    //    EFp_ECD(&T1,&T1);
    for(i=0;r_binary[i]!='\0';i++){
        if(r_binary[i]=='1'){
            EFp_ECA(&T0,&T0,&T1);
            EFp_ECD(&T1,&T1);
        }
        else if(r_binary[i]=='0'){
            EFp_ECA(&T1,&T0,&T1);
            EFp_ECD(&T0,&T0);
        }
    }
    EFp_set(RES,&T0);
    EFp_clear(&T0);
    EFp_clear(&T1);
    return;
}
void EFp3_SCM_ML(struct EFp3 *ANS, struct EFp3 *P,mpz_t scalar){
    count_eca_ML_map = 0;
    count_ecd_ML_map =0;
    int i,length;
    length= (int)mpz_sizeinbase(scalar,2);
    char r_binary[length];
    mpz_get_str(r_binary,2,scalar);
    struct EFp3 T0,T1;
    EFp3_init(&T0);
    
    EFp3_set_infity(&T0);
    EFp3_init(&T1);
    EFp3_set(&T1, P);
    //    EFp_ECD(&T1,&T1);
    for(i=0;r_binary[i]!='\0';i++){
        if(r_binary[i]=='1'){
            EFp3_ECA(&T0,&T0,&T1);
            EFp3_ECD(&T1,&T1);
            count_eca_ML_map ++;
            count_ecd_ML_map ++;
        }
        else if(r_binary[i]=='0'){
            EFp3_ECA(&T1,&T0,&T1);
            EFp3_ECD(&T0,&T0);
            count_eca_ML_map ++;
            count_ecd_ML_map ++;
        }
    }
    EFp3_set(ANS,&T0);
    EFp3_clear(&T0);
    EFp3_clear(&T1);
    return;
}
void EFp6_SCM_ML(struct EFp6 *ANS, struct EFp6 *P,mpz_t scalar){
    int i,length;
    length= (int)mpz_sizeinbase(scalar,2);
    char r_binary[length];
    mpz_get_str(r_binary,2,scalar);
    struct EFp6 T0,T1;
    EFp6_init(&T0);
    
    EFp6_set_infity(&T0);
    EFp6_init(&T1);
    EFp6_set(&T1, P);
    //    EFp_ECD(&T1,&T1);
    for(i=0;r_binary[i]!='\0';i++){
        if(r_binary[i]=='1'){
            EFp6_ECA(&T0,&T0,&T1);
            EFp6_ECD(&T1,&T1);
        }
        else if(r_binary[i]=='0'){
            EFp6_ECA(&T1,&T0,&T1);
            EFp6_ECD(&T0,&T0);
        }
    }
    EFp6_set(ANS,&T0);
    EFp6_clear(&T0);
    EFp6_clear(&T1);
    return;
}
void EFp18_SCM_ML(struct EFp18 *ANS, struct EFp18 *P,mpz_t scalar){
    count_eca_ML = 0;
    count_ecd_ML = 0;
    int i,length;
    length= (int)mpz_sizeinbase(scalar,2);
    char r_binary[length];
    mpz_get_str(r_binary,2,scalar);
    struct EFp18 T0,T1;
    EFp18_init(&T0);
    
    EFp18_set_infity(&T0);
    EFp18_init(&T1);
    EFp18_set(&T1, P);
    //    EFp_ECD(&T1,&T1);
    for(i=0;r_binary[i]!='\0';i++){
        if(r_binary[i]=='1'){
            EFp18_ECA(&T0,&T0,&T1);
            EFp18_ECD(&T1,&T1);
            count_eca_ML ++;
            count_ecd_ML ++;
        }
        else if(r_binary[i]=='0'){
            EFp18_ECA(&T1,&T0,&T1);
            EFp18_ECD(&T0,&T0);
            count_eca_ML ++;
            count_ecd_ML ++;
        }
    }
    EFp18_set(ANS,&T0);
    EFp18_clear(&T0);
    EFp18_clear(&T1);
    return;
}


