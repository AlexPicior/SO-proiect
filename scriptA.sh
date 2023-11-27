#!/bin/bash
regg1="^[A-Z][a-zA-Z0-9, ]*[\.$\!$\?$]"
regg2=",[]*si[]"
regg3="[]*si[],"
count=0
if (test "$#" -eq 1)
then
    while read -r linie;
    do
        rez=$(echo $linie | grep -E "$regg1" | grep -v "$regg2" | grep -v "$regg3" | grep -e "$1")
        if [ -n "$rez" ];
        then
            count=`expr $count + 1`
        fi
    done
    echo $count
else
    echo "Numar incorect de argumente"
fi


