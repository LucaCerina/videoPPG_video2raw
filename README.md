# videoPPG: video to raw signal
Tool that identifies ROIs on human faces and extract raw signal for videoPPG extraction. From `Cerina et al., Influence of acquisition frame-rate and video compression techniques on pulse-rate variability estimation from vPPG signal, 2019` and AFCam project by `Corino et al., Computing in Cardiology (CinC), 2017`

## Dependencies
* QT Creator (tested on v5.7) with compiler to build the project
* OpenCV (tested on v3.1) library
The .pro file should be modified to point to the correct OpenCV folder

## How to use the tool
At the start of the tool, the path of the video and the output name are asked to the user. It is al so asked the camera rotation of the video (if known).

## Original contributors
* Luca Cerina, formerly Politecnico di Milano
* Professor Luca Mainardi, Politecnico di Milano
* Professor Riccardo Barbieri, Politecnico di Milano
* Luca Iozzia PhD, TeiaCare, formerly Politecnico di Milano
* Valentina D.A. Corino PhD, Politecnico di Milano

## Project repositories
* Camera Interface for data collection: [cameraInterface](https://github.com/LucaCerina/videoPPG_cameraInterface)
* Processing of raw RGB signal to Heart Rate Variability [raw2hrv](https://github.com/LucaCerina/videoPPG_raw2hrv)

## To cite this work
If you use this project in a scientific paper, please cite:

```
@article{cerina2019influence,
  title={Influence of acquisition frame-rate and video compression techniques on pulse-rate variability estimation from vPPG signal},
  author={Cerina, Luca and Iozzia, Luca and Mainardi, Luca},
  journal={Biomedical Engineering/Biomedizinische Technik},
  volume={64},
  number={1},
  pages={53--65},
  year={2019},
  publisher={De Gruyter}
}
```
