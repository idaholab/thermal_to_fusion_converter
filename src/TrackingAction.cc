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
/// \file TrackingAction.cc
/// \brief Implementation of the TrackingAction class
//
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "TrackingAction.hh"

#include "Run.hh"
#include "EventAction.hh"
#include "HistoManager.hh"
#include "TrackingMessenger.hh"

#include "G4Track.hh"
#include "G4StepStatus.hh"
#include "G4RunManager.hh"
#include "G4UnitsTable.hh"

#include "G4Neutron.hh"
#include "G4Triton.hh"
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

TrackingAction::TrackingAction(EventAction* evt)
:G4UserTrackingAction(), fTrackMessenger(nullptr),fEventAct(evt),
 fParticleCount(true)
{
  fTrackMessenger = new TrackingMessenger(this); 
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

TrackingAction::~TrackingAction()
{
  delete fTrackMessenger;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void TrackingAction::PreUserTrackingAction(const G4Track* track)
{  
 

  G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();     
  const G4ParticleDefinition* particle = track->GetParticleDefinition();
  G4double energy   = track->GetKineticEnergy();
  G4ThreeVector position = track->GetPosition();
  G4ThreeVector direction = track->GetMomentumDirection();

  if (track->GetTrackID() == 1) {
  //  // primary particle information
  //  G4cout << "energy sampled for incident neutron = " << G4BestUnit(energy,"Energy") << G4endl ; 
  //  G4cout << "position = " << G4BestUnit(position,"Length") << G4endl ;
  //  G4cout << "direction = " << direction << G4endl ;

          analysisManager->FillH1(15,energy); 

  }
  // add secondary particle information 
  else {

      if (particle == G4Triton::Definition())
      {
        analysisManager->FillH1(14,energy); 
      }
      // else if (particle == G4Neutron::Definition() && track->GetParentID()!=1) 
      //   // inelastic reactions (n,n') are secondary particles in geant4 
      //   // skip them to only track neutrons from T-D or T-Li 
      // {

      //   G4cout << "creator model " << track->GetCreatorModelName() << G4endl ;
      //   G4cout << "creator model ID " << track->GetCreatorModelID() << G4endl ;
      //   G4cout << "kinetic energy " << energy << G4endl ;
      //   analysisManager->FillH1(16,energy); 
      // }

      Run* run = static_cast<Run*>(G4RunManager::GetRunManager()->GetNonConstCurrentRun());
        
      G4int iabs        = track->GetTouchableHandle()->GetCopyNumber();
      G4String name     = track->GetDefinition()->GetParticleName();
      G4double meanLife = track->GetDefinition()->GetPDGLifeTime();  
    
      //G4double energy   = track->GetKineticEnergy();
      // get secondary particle generated in LiD
      if (fParticleCount && (iabs == 1))
        run->ParticleCount(iabs,name,energy,meanLife);

  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void TrackingAction::PostUserTrackingAction(const G4Track* track)
{
  // get Run
  Run* run 
    = static_cast<Run*>(
        G4RunManager::GetRunManager()->GetNonConstCurrentRun());
  
  // where are we ?
  G4StepStatus status = track->GetStep()->GetPostStepPoint()->GetStepStatus();
  
  //status of primary particle : absorbed, transmited, reflected ?
  if (track->GetTrackID() == 1) {
    G4int flag = 0;
    if (status == fWorldBoundary) {
      if (track->GetMomentumDirection().x() > 0.) flag = 1;
      else                                        flag = 2;
    }
    run->AddTrackStatus(flag);
  }

  // keep only emerging particles
  if (status != fWorldBoundary) return;

  // count particles
  const G4ParticleDefinition* particle = track->GetParticleDefinition();
  G4String name     = particle->GetParticleName();
  G4double meanLife = particle->GetPDGLifeTime();  
  G4double energy   = track->GetKineticEnergy();
  run->ParticleCount(0,name,energy,meanLife);
  fEventAct->AddEleak(energy);


}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

