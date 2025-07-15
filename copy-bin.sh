#!/bin/bash

cp -r out/rom.bin /Volumes/GENESISSD/rom.bin

if [ $? -ne 0 ]; then
    echo "Error: Failed to copy rom.bin"
    exit 1
fi

echo "Successfully copied rom.bin to /Volumes/GENESISSD/rom.bin"