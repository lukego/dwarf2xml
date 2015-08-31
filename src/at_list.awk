#
# from dwarf-20071016/dwarfdump/at_list.awk
#
# print code to return attribute name from list of attrs in dwarf.h (the input)
# In case of a duplicate value, accept the first as definitive.
# dwarf2 had a couple ambiguities/mistakes in attribute spelling.
BEGIN {
   #printf "struct s_val_2_str {\n  int val;\n char *str;\n};\n"
   printf "struct s_val_2_str c_%stbl[] = {\n", tolower(prefix)
   used_pref["0"] = "";
   # prefix = "DW_FORM_" get from CLI
   prefix_len = length(prefix)
}
$1 ~ "#define" && (substr($2,1,prefix_len) == prefix) {
   
#print "debug : " $0
   p = 0; v = 3; expans = ""
   do {
      p += gsub("[(]",s,$v);
      p -= gsub("[)]",s,$v);
      expans = expans  $v; v++;
   } while ( p > 0 );
#print "debug : " expans
   if ( used_pref[ expans ] != expans )  {
      printf "  { /* %s */ %s, \"%s\" },\n", expans, $2, substr($2,prefix_len+1)
      used_pref [expans] = expans ;
   }
}
END {
   printf "  { 0 },\n"   # last value
   printf "};\n"
}

