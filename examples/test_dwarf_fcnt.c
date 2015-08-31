
enum enum_name { 
  enum_1_val1, 
  enum_1_val2, 
  enum_1_val3 = 8
};
int (*fonct_prt)(int param_a, enum enum_name param_b, short s);
enum enum_name * (*fonct_prt_no_param)();

typedef (*t_fonct_prt)(int param_a, enum enum_name param_b, short s);
int main(int argc, char *argv[], char *envp[]) {
   typedef (*t_pfonct_prt_inner)(char param_a, enum enum_name param_b, short s);
   int (*fonct_prt_inner)(int param_a, enum enum_name param_b, short s);
   int (*fonct_prt_inner_no_param)();
   int fonct_inner(int param_a, enum enum_name param_b, short s) {
       char var_sz_tbl_1[param_a];
       char var_sz_tbl_2[s];
       static t_pfonct_prt_inner fonct_prt_inner;
       int fonct_prt_inner_inner(char param_a, enum enum_name param_b, short s) {
          return param_a;
       }
       fonct_prt_inner = fonct_prt_inner_inner;
       var_sz_tbl_1[param_a -1] = 0;
       var_sz_tbl_2[s -1] = 0;
       return param_a;
   }
   fonct_prt_inner = fonct_inner;
   return 0;
}


