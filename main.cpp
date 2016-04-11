#include <qapplication.h>
#include <QString>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include "viewer.h"

using namespace std;

int main(int argc,char** argv) {
  QApplication application(argc,argv);

  Viewer viewer;

  viewer.setWindowTitle("Projet SIM - POUVARET - TURNEL");
  viewer.show();
  
  return application.exec();
}
