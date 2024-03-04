//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
/// \file HistoManager.cc
/// \brief Implementation of the HistoManager class
//
//
// 
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "HistoManager.hh"
#include "DetectorConstruction.hh"
#include "G4UnitsTable.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

HistoManager::HistoManager()
  : fFileName("spectrum")
{
  Book();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

HistoManager::~HistoManager()
{
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void HistoManager::Book()
{
  // Create or get analysis manager
  // The choice of analysis technology is done via selection of a namespace
  // in HistoManager.hh
  G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
  analysisManager->SetDefaultFileType("csv");
  analysisManager->SetFileName(fFileName);
  analysisManager->SetVerboseLevel(1);
  analysisManager->SetActivation(true);

  // Define histograms start values
  const G4int kMaxHisto = 19;
  const G4String id[] = { "0", "1", "2", "3", "4", "5", "6", "7", "8", "9",
                         "10","11","12","13","14","15","16","17","18","19",
                         "20","21","22"};
  const G4String title[] = 
                { "dummy",                                        //0
                  "total Energy deposited in absorber 1",         //1
                  "total Energy deposited in absorber 2",         //2
                  "total Energy deposited in absorber 3",         //3
                  "total Energy deposited in absorber 4",         //4
                  "total Energy deposited in absorber 5",         //5
                  "total Energy deposited in absorber 6",         //6
                  "total Energy deposited in absorber 7",         //7
                  "total Energy deposited in absorber 8",         //8
                  "total Energy deposited in absorber 9",         //9
                  "Edep (MeV/mm) along absorbers",                //10
                  "total Energy deposited in all absorbers",      //11
		              "total Energy leakage",                         //12
                  "total Energy released",                        //13
                  // custom histograms
                  "Kinetic energy of secondary tritons",            // 14  
                  "Kinetic energy of primary neutrons emitted",     // 15 (use scale252 grid)
                  "Kinetic energy of neutrons generated in D-T reactions", // 16 (use scale252 grid)
                  "Kinetic energy of neutrons generated in T-Li reactions", // 17 (use scale252 grid)
                  "Kinetic energy of neutrons in target"            // 18 (use scale252 grid)
                 };

  // scale 252 energy structure (upper bins)
  const G4int ngroups = 252 ; 
  const std::vector<G4double> scale252= {
1.000e-10, 5.000e-10, 7.500e-10, 1.000e-09, 1.200e-09, 1.500e-09, 2.000e-09, 2.500e-09, 3.000e-09, 4.000e-09, 
5.000e-09, 7.500e-09, 1.000e-08, 2.530e-08, 3.000e-08, 4.000e-08, 5.000e-08, 6.000e-08, 7.000e-08, 8.000e-08, 
9.000e-08, 1.000e-07, 1.250e-07, 1.500e-07, 1.750e-07, 2.000e-07, 2.250e-07, 2.500e-07, 2.750e-07, 3.000e-07, 
3.250e-07, 3.500e-07, 3.750e-07, 4.000e-07, 4.500e-07, 5.000e-07, 5.500e-07, 6.000e-07, 6.250e-07, 6.500e-07, 
7.000e-07, 7.500e-07, 8.000e-07, 8.500e-07, 9.000e-07, 9.250e-07, 9.500e-07, 9.750e-07, 1.000e-06, 1.010e-06, 
1.020e-06, 1.030e-06, 1.040e-06, 1.050e-06, 1.060e-06, 1.070e-06, 1.080e-06, 1.090e-06, 1.100e-06, 1.110e-06, 
1.120e-06, 1.130e-06, 1.140e-06, 1.150e-06, 1.175e-06, 1.200e-06, 1.225e-06, 1.250e-06, 1.300e-06, 1.350e-06, 
1.400e-06, 1.450e-06, 1.500e-06, 1.590e-06, 1.680e-06, 1.770e-06, 1.860e-06, 1.940e-06, 2.000e-06, 2.120e-06, 
2.210e-06, 2.300e-06, 2.380e-06, 2.470e-06, 2.570e-06, 2.670e-06, 2.770e-06, 2.870e-06, 2.970e-06, 3.000e-06, 
3.100e-06, 3.200e-06, 3.500e-06, 3.730e-06, 4.100e-06, 4.700e-06, 5.000e-06, 5.400e-06, 6.000e-06, 6.250e-06, 
6.500e-06, 6.750e-06, 6.875e-06, 7.000e-06, 7.150e-06, 8.100e-06, 9.100e-06, 1.000e-05, 1.150e-05, 1.190e-05, 
1.290e-05, 1.440e-05, 1.600e-05, 1.700e-05, 1.850e-05, 1.940e-05, 2.000e-05, 2.050e-05, 2.120e-05, 2.175e-05, 
2.250e-05, 2.500e-05, 2.750e-05, 3.000e-05, 3.125e-05, 3.175e-05, 3.325e-05, 3.375e-05, 3.500e-05, 3.550e-05, 
3.600e-05, 3.700e-05, 3.713e-05, 3.727e-05, 3.763e-05, 3.800e-05, 3.910e-05, 3.960e-05, 4.100e-05, 4.240e-05, 
4.400e-05, 4.520e-05, 4.830e-05, 5.060e-05, 5.340e-05, 5.800e-05, 6.100e-05, 6.300e-05, 6.500e-05, 6.750e-05, 
7.200e-05, 7.600e-05, 8.000e-05, 8.170e-05, 9.000e-05, 9.700e-05, 1.012e-04, 1.050e-04, 1.080e-04, 1.130e-04, 
1.160e-04, 1.175e-04, 1.190e-04, 1.220e-04, 1.430e-04, 1.700e-04, 1.800e-04, 1.877e-04, 1.885e-04, 1.915e-04, 
1.930e-04, 2.020e-04, 2.074e-04, 2.095e-04, 2.200e-04, 2.400e-04, 2.850e-04, 3.050e-04, 5.500e-04, 6.700e-04, 
6.830e-04, 9.500e-04, 1.150e-03, 1.500e-03, 1.550e-03, 1.800e-03, 2.200e-03, 2.250e-03, 2.500e-03, 3.000e-03, 
3.740e-03, 3.900e-03, 5.700e-03, 8.030e-03, 9.500e-03, 1.300e-02, 1.700e-02, 2.000e-02, 3.000e-02, 4.500e-02, 
5.000e-02, 5.200e-02, 6.000e-02, 7.300e-02, 7.500e-02, 8.200e-02, 8.500e-02, 1.000e-01, 1.283e-01, 1.490e-01, 
2.000e-01, 2.700e-01, 3.300e-01, 4.000e-01, 4.200e-01, 4.400e-01, 4.700e-01, 4.920e-01, 5.500e-01, 5.730e-01, 
6.000e-01, 6.700e-01, 6.790e-01, 7.500e-01, 8.200e-01, 8.611e-01, 8.750e-01, 9.000e-01, 9.200e-01, 1.010e+00, 
1.100e+00, 1.200e+00, 1.250e+00, 1.317e+00, 1.356e+00, 1.400e+00, 1.500e+00, 1.850e+00, 2.354e+00, 2.479e+00, 
3.000e+00, 4.304e+00, 4.800e+00, 6.434e+00, 8.187e+00, 1.000e+01, 1.284e+01, 1.384e+01, 1.455e+01, 1.568e+01, 
1.733e+01, 2.000e+01} ;


  // Default values (to be reset via /analysis/h1/set command)
  G4int nbins = 100;
  G4double vmin = 0.;
  G4double vmax = 100.;

 
  for (G4int k=0; k<kMaxHisto; k++) {
    // Create histograms 1-14 as inactivated as we have not yet set nbins, vmin, vmax
    if (k<=14) {
      G4int ih = analysisManager->CreateH1(id[k], title[k], nbins, vmin, vmax);
      analysisManager->SetH1Activation(ih, false);
    }
    // neutron spectrum - use scale252 energy grid
    else {
      G4int ih = analysisManager->CreateH1(id[k], title[k], scale252);
      analysisManager->SetH1Activation(ih, true);
    }

    
  }


}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
