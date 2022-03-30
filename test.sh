#!/bin/bash

echo "######################"
echo "### Running tests! ###"
echo "######################"

for TEST in $(find tests -iregex '.*\.in' | xargs basename -s .in); do
    echo ""
    IN=tests/$TEST.in
    OUT=tests/$TEST.out
    diff $OUT <(./ymirdb < $IN) && echo "testing $TEST: SUCCESS!" || echo "testing $TEST: FAILURE."
done

echo ""
echo "#######################"
echo "### Finished tests! ###"
echo "#######################"
