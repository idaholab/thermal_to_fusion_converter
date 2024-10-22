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
/// \file EventAction.cc
/// \brief Implementation of the EventAction class
//
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "EventAction.hh"

#include "Run.hh"
#include "HistoManager.hh"

#include "G4Event.hh"
#include "G4EventManager.hh"
#include "G4RunManager.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

EventAction::EventAction(DetectorConstruction* det)
:G4UserEventAction(), fDetector(det)
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

EventAction::~EventAction()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void EventAction::BeginOfEventAction(const G4Event* event)
{
  //energy deposited per event
  for (G4int k=0; k<kMaxAbsor; k++) { fEdepAbsor[k] = 0.0; }
  
  //energy leakage per event
  fEnergyLeak = 0.0;  
  
  //G4int eventID = event->GetEventID(); 
  //G4cout << "Starting event: " << eventID << G4endl ;

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void EventAction::AddEdep(G4int k, G4double edep)
{       
  fEdepAbsor[k] += edep;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void EventAction::AddEleak(G4double eleak)
{       
  fEnergyLeak += eleak;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void EventAction::EndOfEventAction(const G4Event*)
{
  //get Run
  Run* run = static_cast<Run*>(
             G4RunManager::GetRunManager()->GetNonConstCurrentRun());
  
  //plot energy deposited per event
  //
  G4AnalysisManager* analysis = G4AnalysisManager::Instance();
  G4double TotalEdep(0.);
  for (G4int k=1; k<=fDetector->GetNbOfAbsor(); k++) {
    if (fEdepAbsor[k] > 0.) {
      run->AddEdep(k,fEdepAbsor[k]);
      analysis->FillH1(k, fEdepAbsor[k]);
      TotalEdep += fEdepAbsor[k];
    }
  }
  
  if (TotalEdep > 0.) run->AddTotEdep(TotalEdep);
  if (fEnergyLeak > 0.) run->AddEleak(fEnergyLeak);
  G4double Etotal = TotalEdep + fEnergyLeak;
  if (Etotal > 0.) run->AddEtotal(Etotal);
   
  analysis->FillH1(11,TotalEdep);
  analysis->FillH1(12,fEnergyLeak);
  analysis->FillH1(13,Etotal);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

