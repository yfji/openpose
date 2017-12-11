export PATH=/home/yfji/GitProjects/openpose-master_nonstop/3rdparty/caffe/build/lib:$PATH		#for compilation and link
export LD_LIBRARY_PATH=/usr/local/cuda/lib64/:/usr/local/lib/opencv330:/usr/local/lib/boost1.54.0/:/usr/local/lib/mysql:$LD_LIBRARY_PATH				#for run
./build/examples/openpose/openpose.bin --inet_address 172.17.108.58 --server_address 172.17.108.58 --port 8910 --keypoint_port 8912 --num_gpu 1
