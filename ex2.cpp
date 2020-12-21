
#include <cstdlib>
#include <iostream>
#include <map>
#include <string>

#include "TChain.h"
#include "TFile.h"
#include "TTree.h"
#include "TString.h"
#include "TObjString.h"
#include "TSystem.h"
#include "TROOT.h"

#include "TMVA/Factory.h"
#include "TMVA/DataLoader.h"
#include "TMVA/Tools.h"
#include "TMVA/TMVAGui.h"

//void ex2()
//{
    
    // This loads the library
   // TMVA::Tools::Instance();
    
    
    // Read training and test data
 /*
  Data information:
  
  1.  fLength:  continuous  # major axis of ellipse [mm]
  2.  fWidth:   continuous  # minor axis of ellipse [mm]
  3.  fSize:    continuous  # 10-log of sum of content of all pixels [in #phot]
  4.  fConc:    continuous  # ratio of sum of two highest pixels over fSize  [ratio]
  5.  fConc1:   continuous  # ratio of highest pixel over fSize  [ratio]
  6.  fAsym:    continuous  # distance from highest pixel to center, projected onto major axis [mm]
  7.  fM3Long:  continuous  # 3rd root of third moment along major axis  [mm]
  8.  fM3Trans: continuous  # 3rd root of third moment along minor axis  [mm]
  9.  fAlpha:   continuous  # angle of major axis with vector to origin [deg]
  10.  fDist:   continuous  # distance from origin to center of ellipse [mm]
  11.  type:    g,h         # gamma (signal), hadron (background)
  */
    
  /*  auto out = TFile::Open("magic.root" , "RECREATE" );
    
    //Create Trees from the .data file
    ifstream in("magic04.data");
    float Length,Width,Size,Conc,Conc1,Asym,M3Long,M3Trans,Alpha,Dist;
    string sL,sW,sS,sC,sC1,sA,sML,sMT,sAl,sD,type;

    //Create Trees and Branches
    auto signalTree = new TTree("signalTree","signalTree");
    signalTree->Branch("Length",&Length);
    signalTree->Branch("Width",&Width);
    signalTree->Branch("Size",&Size);
    signalTree->Branch("Conc",&Conc);
    signalTree->Branch("Conc1",&Conc1);
    signalTree->Branch("Asym",&Asym);
    signalTree->Branch("M3Long",&M3Long);
    signalTree->Branch("M3Trans",&M3Trans);
    signalTree->Branch("Alpha",&Alpha);
    signalTree->Branch("Dist",&Dist);
    TTree *bkgTree = new TTree("bkgTree","bkgTree");
    bkgTree->Branch("Length",&Length);
    bkgTree->Branch("Width",&Width);
    bkgTree->Branch("Size",&Size);
    bkgTree->Branch("Conc",&Conc);
    bkgTree->Branch("Conc1",&Conc1);
    bkgTree->Branch("Asym",&Asym);
    bkgTree->Branch("M3Long",&M3Long);
    bkgTree->Branch("M3Trans",&M3Trans);
    bkgTree->Branch("Alpha",&Alpha);
    bkgTree->Branch("Dist",&Dist);
    
    //Read data and fill the Trees
    string line;
    while(getline(in,line)){
        stringstream ss(line);
        getline(ss, sW, ',');
        getline(ss, sL, ',');
        getline(ss, sS, ',');
        getline(ss, sC, ',');
        getline(ss, sC1, ',');
        getline(ss, sA, ',');
        getline(ss, sML, ',');
        getline(ss, sMT, ',');
        getline(ss, sAl, ',');
        getline(ss, sD, ',');
        getline(ss, type, '\n');
        Width=stof(sW);
        Length=stof(sL);
        Size=stof(sS);
        Conc=stof(sC);
        Conc1=stof(sC1);
        Asym=stof(sA);
        M3Long=stof(sML);
        M3Trans=stof(sMT);
        Alpha=stof(sAl);
        Dist=stof(sD);
        
        if(type=="g"){
            signalTree->Fill();
    
        }
        else{
            bkgTree->Fill();
        }
        
        
    }
    
    
    signalTree->Write();
    bkgTree->Write();
    out->Close();
    
}*/

void a() {
    
    auto inputFile = TFile::Open("magic.root");
    
    //Length,Width,Size,Conc,Conc1,Asym,M3Long,M3Trans,Alpha,Dist;
    //TFile* outputFile = TFile::Open("TMVAOutputCV.root", "RECREATE");
    
    auto outputFile = TFile::Open( "TMVAOutputCV.root", "RECREATE" );

    TMVA::Factory factory("TMVAClassification", outputFile, "!V:ROC:!Correlations:!Silent:Color:!DrawProgressBar:AnalysisType=Classification");
    
    TMVA::DataLoader loader("dataset");

    loader.AddVariable("Length");
    loader.AddVariable("Width");
    loader.AddVariable("Size");
    loader.AddVariable("Conc");
    loader.AddVariable("Conc1");
    loader.AddVariable("Asym");
    loader.AddVariable("M3Long");
    loader.AddVariable("M3Trans");
    loader.AddVariable("Alpha");
    loader.AddVariable("Dist");
    
    TTree* tsignal;
    TTree* tbackground;
    inputFile->GetObject("signalTree", tsignal);
    inputFile->GetObject("bkgTree", tbackground);

    loader.AddSignalTree(tsignal, 1.0);
    loader.AddBackgroundTree(tbackground, 1.0);
    
    // Apply additional cuts on the signal and background samples
    TCut mycuts = ""; // for example: TCut mycuts = "abs(var1)<0.5 && abs(var2-0.5)<1";
    TCut mycutb = ""; // for example: TCut mycutb = "abs(var1)<0.5";
    
    // Tell the dataloader how to use the training and testing events
    loader.PrepareTrainingAndTestTree(mycuts, mycutb, "NTrain_Signal=0:NTrain_Background=0:NTest_Signal=0:NTest_Background=0");
    
    //Book MVA methods
    //Cuts
        factory.BookMethod( &loader, TMVA::Types::kCuts, "Cuts", "!H:!V:FitMethod=MC:EffSel:SampleSize=200000:VarProp=FSmart" );
    // Fisher discriminant
        factory.BookMethod( &loader, TMVA::Types::kFisher, "Fisher", "H:!V:Fisher:VarTransform=None:CreateMVAPdfs:PDFInterpolMVAPdf=Spline2:NbinsMVAPdf=50:NsmoothMVAPdf=10" );
    //MLPBNN
    //factory.BookMethod(TMVA::Types::kMLP, "MLP", "H:!V:HiddenLayers=3");
        factory.BookMethod( &loader, TMVA::Types::kMLP, "MLPBNN", "H:!V:NeuronType=tanh:VarTransform=N:NCycles=60:HiddenLayers=N+5:TestRate=5:TrainingMethod=BFGS:UseRegulator" );
    // Adaptive Boost
        factory.BookMethod( &loader, TMVA::Types::kBDT, "BDT","NTrees=200:BoostType=AdaBoost");
    
    // Tell the factory to train, test, and evaluate the MVAs
    // Train MVAs using the set of training events
    factory.TrainAllMethods();
    
    // Evaluate all MVAs using the set of test events
    factory.TestAllMethods();
    
    // Evaluate and compare performance of all configured MVAs
    factory.EvaluateAllMethods();
    
    //plot ROC curve
  auto c1 = factory.GetROCCurve(&loader);
  c1->Draw();
    
    // Save the output
  outputFile->Close();
    
    // Launch the GUI for the root macros
    TMVA::TMVAGui("TMVAOutputCV.root");
   

}

