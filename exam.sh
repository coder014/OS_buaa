#!/bin/bash

mkdir mydir
chmod 777 mydir
echo "2023" > myfile
mv moveme mydir/
cp copyme mydir/copied
cat readme
gcc bad.c 2> err.txt
mkdir gen
if (( $# > 0 ))
then
	n=$1
else
	n=10
fi
i=1
while (($i <= $n))
do
	touch gen/$i.txt
	let i=i+1
done
