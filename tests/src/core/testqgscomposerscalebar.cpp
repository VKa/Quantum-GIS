/***************************************************************************
                         testqgscomposerscalebar.cpp
                         ---------------------------
    begin                : August 2012
    copyright            : (C) 2012 by Marco Hugentobler
    email                : marco at sourcepole dot ch
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "qgsapplication.h"
#include "qgscomposition.h"
#include "qgscompositionchecker.h"
#include "qgscomposermap.h"
#include "qgscomposerscalebar.h"
#include "qgsmaplayerregistry.h"
#include "qgsmaprenderer.h"
#include "qgsmultibandcolorrenderer.h"
#include "qgsrasterlayer.h"
#include <QObject>
#include <QtTest>

class TestQgsComposerScaleBar: public QObject
{
    Q_OBJECT;
  private slots:
    void initTestCase();// will be called before the first testfunction is executed.
    void cleanupTestCase();// will be called after the last testfunction was executed.
    void init();// will be called before each testfunction is executed.
    void cleanup();// will be called after every testfunction.
    void singleBox();
    void doubleBox();
    void numeric();
    void tick();

  private:
    QgsComposition* mComposition;
    QgsComposerMap* mComposerMap;
    QgsComposerScaleBar* mComposerScaleBar;
    QgsRasterLayer* mRasterLayer;
    QgsMapRenderer* mMapRenderer;
    QString mReport;
};

void TestQgsComposerScaleBar::initTestCase()
{
  QgsApplication::init();
  QgsApplication::initQgis();

  //create maplayers from testdata and add to layer registry
  QFileInfo rasterFileInfo( QString( TEST_DATA_DIR ) + QDir::separator() +  "landsat.tif" );
  mRasterLayer = new QgsRasterLayer( rasterFileInfo.filePath(),
                                     rasterFileInfo.completeBaseName() );
  QgsMultiBandColorRenderer* rasterRenderer = new QgsMultiBandColorRenderer( mRasterLayer->dataProvider(), 2, 3, 4 );
  mRasterLayer->setRenderer( rasterRenderer );

  QgsMapLayerRegistry::instance()->addMapLayers( QList<QgsMapLayer*>() << mRasterLayer );

  //create composition with composer map
  mMapRenderer = new QgsMapRenderer();
  mMapRenderer->setLayerSet( QStringList() << mRasterLayer->id() );

  //reproject to WGS84
  QgsCoordinateReferenceSystem destCRS;
  destCRS.createFromId( 4326, QgsCoordinateReferenceSystem::EpsgCrsId );
  mMapRenderer->setDestinationCrs( destCRS );
  mMapRenderer->setProjectionsEnabled( true );

  mComposition = new QgsComposition( mMapRenderer );
  mComposition->setPaperSize( 297, 210 ); //A4 landscape
  mComposerMap = new QgsComposerMap( mComposition, 20, 20, 150, 150 );
  mComposerMap->setFrameEnabled( true );
  mComposition->addComposerMap( mComposerMap );
  mComposerMap->setNewExtent( QgsRectangle( 17.923, 30.160, 18.023, 30.260 ) );

  mComposerScaleBar = new QgsComposerScaleBar( mComposition );
  mComposerScaleBar->setSceneRect( QRectF( 20, 180, 20, 20 ) );
  mComposition->addComposerScaleBar( mComposerScaleBar );
  mComposerScaleBar->setComposerMap( mComposerMap );
  mComposerScaleBar->setUnits( QgsComposerScaleBar::Meters );
  mComposerScaleBar->setNumUnitsPerSegment( 2000 );
  mComposerScaleBar->setNumSegmentsLeft( 0 );
  mComposerScaleBar->setNumSegments( 2 );
  mComposerScaleBar->setHeight( 5 );

  qWarning() << "scalebar font: " << mComposerScaleBar->font().toString() << " exactMatch:" << mComposerScaleBar->font().exactMatch();

  mReport = "<h1>Composer Scalebar Tests</h1>\n";
}

void TestQgsComposerScaleBar::cleanupTestCase()
{
  delete mComposition;
  delete mMapRenderer;
  delete mRasterLayer;

  QString myReportFile = QDir::tempPath() + QDir::separator() + "qgistest.html";
  QFile myFile( myReportFile );
  if ( myFile.open( QIODevice::WriteOnly | QIODevice::Append ) )
  {
    QTextStream myQTextStream( &myFile );
    myQTextStream << mReport;
    myFile.close();
  }
};

void TestQgsComposerScaleBar::init()
{

};

void TestQgsComposerScaleBar::cleanup()
{

};

void TestQgsComposerScaleBar::singleBox()
{
  mComposerScaleBar->setStyle( "Single Box" );
  QgsCompositionChecker checker( "composerscalebar_singlebox", mComposition );
  QVERIFY( checker.testComposition( mReport ) );
};

void TestQgsComposerScaleBar::doubleBox()
{
  mComposerScaleBar->setStyle( "Double Box" );
  QgsCompositionChecker checker( "composerscalebar_doublebox", mComposition );
  QVERIFY( checker.testComposition( mReport ) );
};

void TestQgsComposerScaleBar::numeric()
{
  mComposerScaleBar->setStyle( "Numeric" );
  QgsCompositionChecker checker( "composerscalebar_numeric", mComposition );
  QVERIFY( checker.testComposition( mReport ) );
};

void TestQgsComposerScaleBar::tick()
{
  mComposerScaleBar->setStyle( "Line Ticks Up" );
  QgsCompositionChecker checker( "composerscalebar_tick", mComposition );
  QVERIFY( checker.testComposition( mReport ) );
};

QTEST_MAIN( TestQgsComposerScaleBar )
#include "moc_testqgscomposerscalebar.cxx"
