#! /usr/bin/bash

[ $# -lt 1 ] && { echo "USAGE: $(basename $0) cmde args..." 1>&2; exit 1; }
[ ! -x $1 ] && { echo "$(basename $0): cmde \"$1\" does not exist or is not executable" 1>&2; exit 1; }
echo "cmde ready"
# Hit return to read a newline from command output
exec 4<&0
stty -echo
$* 2>/dev/null | \
   while read <&4; do { 
     while read || break 2; do { 
         [ "$REPLY" != "" ] && { echo "$REPLY"; break; }
     } done
   } done
stty echo

