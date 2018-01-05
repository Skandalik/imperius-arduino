#!/bin/sh
# for i in {1..4}; do python client.py&; done
for i in {1..2}
do
    python adjustable-client.py&
done

for i in {1..2}
do
    python fetchable-client.py&
done

for i in {1..2}
do
    python switchable-client.py&
done