#!/bin/bash

myresults="samples/decafMyResults.txt"
hisresults="samples/decafHisResults.txt"

lineSeperator="--------------------------------------------------------------"
find samples/ -name "*.decaf" | sort > samples/decafFiles.txt
file=samples/decafFiles.txt

echo "Decaf Test Results Sheet" > $myresults
echo $lineSeperator > $myresults

echo "Decaf Test Results Sheet" > $hisresults
echo $lineSeperator > $hisresults

while IFS= read FILENAME
do
        echo "Testing $FILENAME"
        echo $FILENAME >> $myresults
        ./dcc<$FILENAME &>> $myresults 
        echo $FILENAME >> $hisresults
        solutions/dcc<$FILENAME &>> $hisresults 
done <"$file"