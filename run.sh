<<<<<<< HEAD
#! \bin\bash

$pre_command ./build/src/mem_map
if [ $? -ne 0 ]; then
    curl -X POST --data "payload={\"icon_emoji\": \":jenkins:\", \"username\": \"jenkins\"  , \"attachments\":[{ \"title\":\"Error:\", \"color\": \"#FF0000\", \"text\":\"$2 failed the test with openfpm_data test $opt_comp \" }] }" https://hooks.slack.com/services/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
    exit 1
fi

 
=======
#! /bin/bash

hostname=$(hostname)
branch=$3

# Make a directory in /tmp/openfpm_data

cd "openfpm_io"

echo "CHECKING MACHINE"
if [ x"$hostname" == x"cifarm-centos-node.mpi-cbg.de"  ]; then
	echo "CENTOS"
        export LD_LIBRARY_PATH="$LD_LIBRARY_PATH:$HOME/openfpm_dependencies/openfpm_io/$branch/HDF5/lib"
	export LD_LIBRARY_PATH="$LD_LIBRARY_PATH:$HOME/openfpm_dependencies/openfpm_io/$branch/BOOST/lib"
fi

if [ x"$hostname" == x"cifarm-ubuntu-node"  ]; then
        export LD_LIBRARY_PATH="$LD_LIBRARY_PATH:$HOME/openfpm_dependencies/openfpm_io/$branch/HDF5/lib"
	export LD_LIBRARY_PATH="$LD_LIBRARY_PATH:$HOME/openfpm_dependencies/openfpm_io/$branch/BOOST/lib"
fi

if [ x"$hostname" == x"cifarm-mac-node.mpi-cbg.de"  ]; then
	echo "MACOS X"
        export DYLD_LIBRARY_PATH="$DYLD_LIBRARY_PATH:$HOME/openfpm_dependencies/openfpm_io/$branch/HDF5/lib"
	export DYLD_LIBRARY_PATH="$DYLD_LIBRARY_PATH:$HOME/openfpm_dependencies/openfpm_io/$branch/BOOST/lib"
fi

pwd

./build/src/io


>>>>>>> origin/master
