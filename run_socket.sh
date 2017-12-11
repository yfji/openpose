export PATH=/home/yfji/GitProjects/openpose-master_allserver/3rdparty/caffe/build/lib:$PATH		#for compilation and link
export LD_LIBRARY_PATH=/usr/local/lib/opencv_310:$LD_LIBRARY_PATH				#for run
./build/examples/openpose/openpose.bin --inet_address 172.17.108.58 --server_address 172.17.108.58 --port 8976 --keypoint_port 8977 --no_display true --no_gui_verbose true --num_gpu 1
# ./build/examples/openpose/openpose.bin --inet_address 10.106.20.8 --server_address 10.106.20.8 --num_gpu 2
