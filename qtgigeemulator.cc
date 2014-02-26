// Copyright (c) 2011 University of Southern Denmark. All rights reserved.
// Use of this source code is governed by the MIT license (see license.txt).
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <semaphore.h>
#include <QElapsedTimer>
#include <QDialog>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QGridLayout>
#include <QLabel>
#include <QComboBox>
#include <QLineEdit>
#include <QSlider>
#include <QPushButton>
#include <qcheckbox.h>
#include <QDateTime>
#include <cstdio>
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>

#include "qtgigeemulator.moc"
#include "qtgigeemulator.h"

#define CV_LOAD_IMAGE_GRAYSCALE_IS_DEFINED

QTGIGEEmulator::QTGIGEEmulator(const char* deviceId)
{
  updateptimer = false;
//   std::cout << "Vendor name:" << arv_camera_get_vendor_name (camera) << std::endl;
//   std::cout << "Model name:" << arv_camera_get_model_name (camera) << std::endl;
//   std::cout << "Device ID:" << arv_camera_get_device_id (camera) << std::endl;
  
  abort = false;
  this->drawSettingsDialog();
  roi_width = 546;
  roi_height = 586;
  roi_x = 581;
  roi_y = 1;
  roi_scale = 1.0;
  roi_cpos = 0;
  this->start();
}

void QTGIGEEmulator::setptimer(itimerval timer)
{
  setitimer(ITIMER_PROF, &timer, NULL	);
  ptimer = timer;
  updateptimer = true;
}

QTGIGEEmulator::~QTGIGEEmulator()
{
  abort = true;
  this->msleep(300);
}

void QTGIGEEmulator::showCameraSettings(void )
{
  PrintParms();
}

void QTGIGEEmulator::drawSettingsDialog(void )
{
  settings = new QDialog();
  settings->hide();
  treeWidget = new QTreeWidget(settings);
  currentSetting = new QWidget(settings);
  treeWidget->setColumnCount(2);
  QStringList headers;
  headers.append(QString("Feature"));
  headers.append(QString("Value"));
  treeWidget->setHeaderLabels(headers);
  settings->setWindowTitle(QString("Camera settings"));
  settingsLayout = new QGridLayout(settings);
//  treeWidget->setMinimumSize(600, 800);
  treeWidget->setColumnWidth(0, 300);
  connect(treeWidget, SIGNAL(itemClicked(QTreeWidgetItem*,int)), SLOT(newSettingSelected(QTreeWidgetItem*,int)));
  connect(settings, SIGNAL(finished(int)), treeWidget, SLOT(clear()));
  connect(treeWidget, SIGNAL(itemClicked(QTreeWidgetItem*,int)), treeWidget, SLOT(update()));
//  currentSetting->setMinimumSize(600,800);
  settingsLayout->addWidget(treeWidget,1,1);
  settingsLayout->addWidget(currentSetting, 1,2);
  treeWidget->setMinimumHeight(600);
  treeWidget->setMinimumWidth(500);
  settings->setLayout(settingsLayout);
  currentSettingLayout = new QGridLayout(currentSetting);
  currentSetting->setLayout(currentSettingLayout);
  settings->hide();
}


void QTGIGEEmulator::PrintParms(void )
{
  treeWidget->expandItem(treeWidget->topLevelItem(0));
  settings->show();
}

void QTGIGEEmulator::newSettingSelected(QTreeWidgetItem* item, int column)
{
}

void QTGIGEEmulator::writeInt(QString nodeName, int value)
{
  std::cout << "Received request to set " << nodeName.toLocal8Bit().constData() << " to " << value << std::endl;
}

void QTGIGEEmulator::writeBool(QString nodeName, bool value)
{
  std::cout << "Received request to set " << nodeName.toLocal8Bit().constData() << " to " << value << std::endl;
}

void QTGIGEEmulator::emitAction(QString nodeName)
{
  std::cout << "Received request to action " << nodeName.toLocal8Bit().constData() << std::endl;
}

void QTGIGEEmulator::emitActionFromSettings(void )
{
  QTreeWidgetItem * item = (QTreeWidgetItem *) this->sender()->property("nodeItem").value<void *>();
  QString nodeName = item->text(0);
  emitAction(nodeName);
}


void QTGIGEEmulator::writeBoolFromSettings(int value)
{
  QTreeWidgetItem * item = (QTreeWidgetItem *) this->sender()->property("nodeItem").value<void *>();
  QString nodeName = item->text(0);
  QLineEdit * codeSnippet = (QLineEdit *) this->sender()->property("codeSnippet").value<void *>();
  if(value==Qt::Checked)
  {
    item->setText(1, "True");
    codeSnippet->setText(QString("QTGIGEEmulator::writeBool(\"") + item->text(0) + QString("\", true);"));
    writeBool(nodeName, true);
  }
  else
  {
    item->setText(1, "False");
    codeSnippet->setText(QString("QTGIGEEmulator::writeBool(\"") + item->text(0) + QString("\", false);"));
    writeBool(nodeName, false);
  }
}


void QTGIGEEmulator::writeIntFromSettings(int value)
{
  QTreeWidgetItem * item = (QTreeWidgetItem *) this->sender()->property("nodeItem").value<void *>();
  QString nodeName = item->text(0);
  QLineEdit * codeSnippet = (QLineEdit *) this->sender()->property("codeSnippet").value<void *>();
  item->setText(1, QString::number((((float)value))));
  codeSnippet->setText(QString("QTGIGEEmulator::writeInt(\"") + item->text(0) + QString("\", ") + QString::number(value) + QString(");"));
  writeInt(nodeName, value);
}


void QTGIGEEmulator::writeFloat(QString nodeName, float value)
{
  std::cout << "Received request to set " << nodeName.toLocal8Bit().constData() << " to " << value << std::endl;
}

void QTGIGEEmulator::writeFloatFromSettings(int value)
{
  QTreeWidgetItem * item = (QTreeWidgetItem *) this->sender()->property("nodeItem").value<void *>();
  QString nodeName = item->text(0);
  QLineEdit * codeSnippet = (QLineEdit *) this->sender()->property("codeSnippet").value<void *>();
  float mul = this->sender()->property("multiplier").toFloat();
  item->setText(1, QString::number((((float)value)/mul)));
  writeFloat(nodeName, (((float)value)/mul));
}


void QTGIGEEmulator::writeEnumFromSettingsSelectorMapper(QString value)
{
  QString nodeName(this->sender()->property("nodeName").toString());
  QTreeWidgetItem * item = (QTreeWidgetItem *) this->sender()->property("nodeItem").value<void *>();
  QLineEdit * codeSnippet = (QLineEdit *) this->sender()->property("codeSnippet").value<void *>();
  codeSnippet->setText(QString("QTGIGEEmulator::writeEnum(\"") + nodeName + QString("\", \"") + value + QString("\");"));
  item->setText(1, value);
  writeEnum(nodeName, value);
}


void QTGIGEEmulator::writeEnum(QString nodeName, QString value)
{
  std::cout << "Received request to set " << nodeName.toLocal8Bit().constData() << " to " << value.toLocal8Bit().constData() << std::endl;
}


int64 QTGIGEEmulator::getSensorWidth()
{
    QString nodeName = "WidthMax";
    return -1;
}

int64 QTGIGEEmulator::getSensorHeight()
{
    QString nodeName = "HeightMax";
    return -1;
}

int QTGIGEEmulator::setROI(int x, int y, int width, int height)
{
}

int QTGIGEEmulator::setExposure(float period)
{
}

int QTGIGEEmulator::setGain(float gain)
{
}

void QTGIGEEmulator::convert16to8bit(cv::InputArray in, cv::OutputArray out)
{
  uint16_t * in_ = (uint16_t*)in.getMat().ptr();
  cv::Mat tmp_out(in.getMat().size().height, in.getMat().size().width, cv::DataType<uint8_t>::type);
  uint8_t * out_ = tmp_out.ptr();
  uint16_t * end = in_ + (in.getMat().size().height * in.getMat().size().width);
  while(in_!=end)
  {
    *out_++ = (*in_++)>>8;
  }
  tmp_out.copyTo(out);
}

void QTGIGEEmulator::convert8to16bit(cv::InputArray in, cv::OutputArray out)
{
  uint8_t * in_ = (uint8_t*)in.getMat().ptr();
  cv::Mat tmp_out(in.getMat().size().height, in.getMat().size().width, cv::DataType<uint16_t>::type);
  uint16_t * out_ = (uint16_t*)tmp_out.ptr();
  uint8_t * end = in_ + (in.getMat().size().height * in.getMat().size().width);
  while(in_!=end)
  {
    *out_++ = (*in_++)<<8;
  }
  tmp_out.copyTo(out);
}

int QTGIGEEmulator::startAquisition(void )
{
}


int QTGIGEEmulator::stopAquisition(void )
{

}

void QTGIGEEmulator::run()
{
  std::cout << "Basler_AVA2000 TID:" << syscall(SYS_gettid) << std::endl << std::flush;
  char * tmp= (char*)malloc(1000*2000*5);
//  posix_memalign((void**)(&tmp),8,1000*2000*5); //alloc aligned memmory
  nFrames = 0;  
  successFrames = 0;
  failedFrames = 0;
  
  cv::Mat emu_image;
  std::cout << "Using " << EMULATION_INPUT_FILE << " as input file for emulation" << std::endl;
  #ifdef CV_LOAD_IMAGE_GRAYSCALE_IS_DEFINED
  emu_image = cv::imread(EMULATION_INPUT_FILE, CV_LOAD_IMAGE_GRAYSCALE);
  #else
  emu_image = cv::imread(EMULATION_INPUT_FILE, cv::IMREAD_GRAYSCALE);
  #endif
  cv::transpose(emu_image, emu_image);
  std::cout << "Emulation image size " << emu_image.size().width << "x" << emu_image.size().height << "x" << emu_image.channels() << std::endl;
  unsigned int length = emu_image.size().height;
  
  framePeriod.start();
  while(abort==false)
  {
    if(nFrames >=frameAvg)
    {
      nFrames = 0;
      float fps = framePeriod.elapsed();
      fps /= frameAvg;
      fps = 1000.0f/fps;
      emit(measuredFPS(fps));
      emit(measuredFrameStats(successFrames, failedFrames));
      framePeriod.restart();
    }
    else
    {
      nFrames++;
    }
    if(updateptimer)
    {
      setitimer(ITIMER_PROF, &ptimer, NULL);
      updateptimer = false;
    }

   //Send out next emulated frame
   roi_cpos -= roi_height/10;
   if(roi_cpos-roi_height<0)
     roi_cpos = length;
   cv::Mat subImg = emu_image(cv::Range(roi_cpos-roi_height, roi_cpos), cv::Range(roi_x, roi_x+roi_width));
//   cv::Mat subImg16;
//   convert8to16bit(subImg, subImg16);
  cv::Mat RGB161616(roi_height,roi_width, cv::DataType<uint16_t>::type);
  subImg.convertTo(RGB161616, RGB161616.type(), 256.0);
  //Set red and blue in bayer pattern = 0
  uint16_t h = RGB161616.size().height;
  uint16_t w = RGB161616.size().width;
  uint16_t* ptr = (uint16_t*)RGB161616.ptr();
  for(uint16_t y = 0; y < h; y+=2)
  {
    for(uint16_t x = 1; x < w; x+=2)
    {
//       ptr[y*w+x] = 0;
    }
  }
  for(uint16_t y = 1; y < h; y+=2)
  {
    for(uint16_t x = 0; x < w; x+=2)
    {
//       ptr[y*w+x] = 0;
    }
  }
  //  std::cout << "RGB161616 image size " << RGB161616.size().width << "x" << RGB161616.size().height << "x" << RGB161616.channels() << std::endl;
  //cv::imwrite("test.png", RGB161616);
   //emit(this->newBayerGRImage(RGB161616, QDateTime::currentMSecsSinceEpoch()*1000));
   emit(this->newBayerGRImage(RGB161616, roi_cpos));
   this->msleep(300);
  }
}

void QTGIGEEmulator::loadCorrectionImage(const QString pathToLog)
{
  cv::Mat tempImage;
  #ifdef CV_LOAD_IMAGE_GRAYSCALE_IS_DEFINED
  tempImage = cv::imread(pathToLog.toStdString(), CV_LOAD_IMAGE_GRAYSCALE);
  #else
  tempImage = cv::imread(pathToLog.toStdString(), cv::IMREAD_GRAYSCALE);
  #endif
  convert8to16bit(tempImage, correctionImage);
}

void QTGIGEEmulator::correctVignetting(cv::Mat img, qint64 timestampus)
{
//  std::cout << "img.cols: " << img.cols << " correctionImage.cols: " << correctionImage.cols << std::endl;
//  std::cout << "img.rows: " << img.rows << " correctionImage.rows: " << correctionImage.rows << std::endl;
  assert(img.cols == correctionImage.cols);
  assert(img.rows == correctionImage.rows);
  
  cv::Mat resultImage = img.mul(correctionImage, 1 / 64000.);
  emit(vignettingCorrectedInImage(resultImage, timestampus));
//  std::cout << "Emitted vignetting correcting image" << std::endl;
}