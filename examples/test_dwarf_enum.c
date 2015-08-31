
typedef char _type_char_;
typedef char *_type_char_ptr;

enum enum_name { 
  enum_1_val1, 
  enum_1_val2, 
  enum_1_val3 = 8
} var_named_enum, *pvar_named_enum;
const enum enum_name * cpvar_named_enum;
enum enum_name * const pcvar_named_enum;
volatile enum enum_name *vpvar_named_enum;
enum enum_name * volatile pvvar_named_enum;
const volatile enum enum_name *cvpvar_named_enum;
volatile const enum enum_name *vcpvar_named_enum;
const enum enum_name * volatile cpvvar_named_enum;
volatile enum enum_name * const vpcvar_named_enum;
enum enum_name * const volatile pcvvar_named_enum;
enum enum_name * volatile const pvcvar_named_enum;

enum { 
  enum_2_val1, 
  enum_2_val2, 
  enum_2_val3 = 8
} var_unnamed_enum, *pvar_unnamed_enum,  * const pcvar_unnamed_enum,  * volatile pvvar_unnamed_enum;
const enum { 
  enum_3_val_1, 
  enum_3_val2, 
  enum_3_val3 = 8
} *cpvar_unnamed_enum;

int main(int argc, char *argv[], char *envp[]) {
   enum enum_name * volatile pvvar_named_enum;
   pvvar_named_enum = 0;
   
   return 0;
}


