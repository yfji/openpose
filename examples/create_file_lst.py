# -*- coding: utf-8 -*-
"""
Created on Thu Nov  2 14:43:45 2017

@author: yfji
"""

import os

root=os.getcwd()
image_dir=os.path.join(root, 'all_images')
image_files=os.listdir(image_dir)

f=open('file_lst.txt','w')
for image_file in image_files:
    f.write(os.path.join(image_dir,image_file)+'\n')

f.close()