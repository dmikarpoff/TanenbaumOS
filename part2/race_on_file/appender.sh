#!/bin/bash

file_name="x.txt"

if test -f "$file_name"; then
  rm "$file_name"
fi

echo "0" >> $file_name

while true
do
  while true ; do
    ln -s $file_name $file_name.lock 2> /dev/null
    [ $? -ne 0 ] || break
  done
  val=`tail -1 $file_name`
  echo "$val"
  val=$((val+1))
  echo "$val" >> $file_name
  rm $file_name.lock
done
