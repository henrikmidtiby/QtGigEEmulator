// Copyright (c) 2011 University of Southern Denmark. All rights reserved.
// Use of this source code is governed by the MIT license (see license.txt).
#ifndef QTGIGEEmulator_H
#define QTGIGEEmulator_H
#define EMULATION_INPUT_FILE "../simulation_image/maizeAnd3weedsCombinedImage.png"
#include <QObject>
#include <QMutex>
#include <QQueue>
#include <QSemaphore>
#include <QElapsedTimer>
#include <QTreeWidgetItem>
#include <QGridLayout>
#include <QDialog>

#include <time.h>
#include <sys/time.h>
#include <QThread>
#include <qvarlengtharray.h>
#include <qsignalmapper.h>
#include <opencv2/opencv.hpp>
#include <opencv/highgui.h>
  class QTGIGEEmulator : public QThread {
    Q_OBJECT
    public:
    QTGIGEEmulator(const char* deviceId);
    ~QTGIGEEmulator();
    int setROI(int x, int y, int width, int height);
    int setExposure(float period); //Exposure time in µs
    int setGain(float gain); //Unit currently unknown (have to look it up from datasheet)
    int startAquisition(void);
    int stopAquisition(void);
    void setptimer(itimerval timer);
    void PrintParms(void);
    static void convert16to8bit(cv::InputArray in, cv::OutputArray out);
    static void convert8to16bit(cv::InputArray in, cv::OutputArray out);
    void loadCorrectionImage(const QString pathToLog);
  signals:
    void newBayerGRImage(const cv::Mat img, qint64 timestampus);
    void measuredFPS(float fps);
    void measuredFrameStats(int success, int failed);
    void vignettingCorrectedInImage(const cv::Mat img, qint64 timestampus);
  public slots:
    void showCameraSettings(void);
    void writeEnum(QString nodeName, QString value);
    void writeFloat(QString nodeName, float value);
    void writeInt(QString nodeName, int value);
    void writeBool(QString nodeName, bool value);
    void emitAction(QString nodeName);
    void correctVignetting(cv::Mat img, qint64 timestampus);
  private:
      void run();
      int roi_cpos;
      int roi_width;
      int roi_height;
      int roi_x;
      int roi_y;
      double roi_scale;
      QSemaphore bufferSem;
      itimerval ptimer;
      bool updateptimer;
      bool abort;
      static const int frameAvg = 20;
      QElapsedTimer framePeriod;
      int nFrames;
      int successFrames;
      int failedFrames;
      cv::Mat correctionImage;
      QDialog *settings;
      QWidget * currentSetting;
      QTreeWidget *treeWidget;
      QGridLayout *settingsLayout;
      QGridLayout * currentSettingLayout;
      void drawSettingsDialog(void);
      int64 getSensorHeight();
      int64 getSensorWidth();
  private slots:
      void newSettingSelected(QTreeWidgetItem* item,int column);
      void writeEnumFromSettingsSelectorMapper(QString value);
      void writeFloatFromSettings(int value);
      void writeIntFromSettings(int value);
      void writeBoolFromSettings(int value);
      void emitActionFromSettings(void);
  };

#endif  // QTGIGEEmulator_H

