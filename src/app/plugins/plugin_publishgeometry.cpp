//========================================================================
//  This software is free: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License Version 3,
//  as published by the Free Software Foundation.
//
//  This software is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  Version 3 in the file COPYING that came with this distribution.
//  If not, see <http://www.gnu.org/licenses/>.
//========================================================================
/*!
  \file    plugin_publishgeometry.cpp
  \brief   C++ Implementation: plugin_publishgeometry
  \author  Author Name, 2009
*/
//========================================================================
#include "plugin_publishgeometry.h"

PluginPublishGeometry::PluginPublishGeometry(FrameBuffer * fb, RoboCupSSLServer * server, const RoboCupField & field)
 : VisionPlugin(fb), _field(field)
{
  _server=server;
  setSharedAmongStacks(true);
  _settings=new VarList("Publish Geometry");
  _settings->addChild(_pub=new VarTrigger("Publish","Publish!"));
  connect(_pub,SIGNAL(signalTriggered()),this,SLOT(slotPublishTriggered()));
}

void PluginPublishGeometry::addCameraParameters(CameraParameters * param) {
  lock();
  params.push_back(param);
  unlock();
}

PluginPublishGeometry::~PluginPublishGeometry()
{
  delete _settings;
  delete _pub;
}

VarList * PluginPublishGeometry::getSettings() {
  return _settings;
}

string PluginPublishGeometry::getName() {
  return "Publish Geometry";
}

void PluginPublishGeometry::sendGeometry() {
  SSL_GeometryData geodata;
  SSL_GeometryFieldSize * gfield = geodata.mutable_field();
  _field.toProtoBuffer(*gfield);
  for (unsigned int i = 0; i < params.size(); i++) {
    SSL_GeometryCameraCalibration * calib = geodata.add_calib();
    params[i]->toProtoBuffer(*calib,i);
  }
  _server->send(geodata);
}

void PluginPublishGeometry::slotPublishTriggered() {
  lock();
    sendGeometry();
  unlock();
}

ProcessResult PluginPublishGeometry::process(FrameData * data, RenderOptions * options) {
  (void)data;
  (void)options;
  //TODO: check client requests in server process
  //      if requested, call sendGeometry();

  return ProcessingOk;
}

