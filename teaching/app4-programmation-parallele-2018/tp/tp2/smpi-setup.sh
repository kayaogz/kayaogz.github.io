#!/bin/bash
cd ~/
mkdir -p simgrid
cd simgrid
ls
wget https://gforge.inria.fr/frs/download.php/file/35817/SimGrid-3.13.tar.gz
tar -xf SimGrid-3.13.tar.gz
cd SimGrid-3.13
cmake -Denable_documentation=0 .
make -j4
cd smpi_script/bin
echo "export PATH=\$PATH:$(pwd)" >> ~/.bashrc
