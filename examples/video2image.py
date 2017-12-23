# -*- coding: utf-8 -*-
"""
Created on Sun Oct 29 21:50:29 2017

@author: yfji
"""

import cv2
import os

root=os.getcwd()
video_dir=os.path.join(root, 'single_person.mp4')
image_dir=os.path.join(root, 'images')

cap=cv2.VideoCapture(video_dir)
print(cap.get(cv2.CAP_PROP_FPS))
frameIndex=0

while True:
    ok, frame=cap.read()
    print(frame.shape)
    if not ok:
        break
    cv2.imshow('frame', frame)
    cv2.imwrite(os.path.join(image_dir, 'frame_%d.jpg'%frameIndex), frame)
    if cv2.waitKey(10)==27:
        break
    frameIndex+=1

cv2.destroyAllWindows()