#!/bin/sh

cp -r ../res ../build/

# Change to the build directory
cd ../build || { echo "Failed to enter build directory"; exit 1; }

# Run the make command
make || { echo "Make failed"; exit 1; }

./game || { echo "Failed to execute ./game"; exit 1; }

# Return to the original directory
cd ../src
