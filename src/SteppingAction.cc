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
/// \file SteppingAction.cc
/// \brief Implementation of the SteppingAction class
//
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "SteppingAction.hh"
#include "DetectorConstruction.hh"
#include "EventAction.hh"
#include "Run.hh"
#include "HistoManager.hh"

#include "G4SteppingManager.hh"
#include "G4RunManager.hh"
#include "Randomize.hh"
#include "G4UnitsTable.hh"


#include "G4HadronicProcess.hh"
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

SteppingAction::SteppingAction(DetectorConstruction* det, EventAction* event)
:G4UserSteppingAction(),fDetector(det), fEventAction(event)
{ }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

SteppingAction::~SteppingAction()
{ }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void SteppingAction::UserSteppingAction(const G4Step* step)
{


  // count processes
  // 
  const G4StepPoint* endPoint = step->GetPostStepPoint();
  G4VProcess* process   = const_cast<G4VProcess*>(endPoint->GetProcessDefinedStep());
  Run* run = static_cast<Run*>(
        G4RunManager::GetRunManager()->GetNonConstCurrentRun());
  run->CountProcesses(process);
  
 G4double edep = step->GetTotalEnergyDeposit();



  //energy-momentum balance initialisation
  //
  const G4StepPoint* prePoint = step->GetPreStepPoint();
  G4double Q             = - prePoint->GetKineticEnergy();
  G4ThreeVector Pbalance = - prePoint->GetMomentum();



  //initialisation of the nuclear channel identification
  //
  G4ParticleDefinition* particle = step->GetTrack()->GetDefinition();
  G4String partName = particle->GetParticleName();
  G4String nuclearChannel = partName;
  G4HadronicProcess* hproc = dynamic_cast< G4HadronicProcess*>(process);
  const G4Isotope* target = NULL;
  if (hproc) target = hproc->GetTargetIsotope();
  G4String targetName = "XXXX";  
  if (target) targetName = target->GetName();
  nuclearChannel += " + " + targetName + " --> ";
  if (targetName == "XXXX") run->SetTargetXXX(true);

  //scattered primary particle (if any)
  //
  G4AnalysisManager* analysis = G4AnalysisManager::Instance();
  G4int ih = 1;
  if (step->GetTrack()->GetTrackStatus() == fAlive) {
    G4double energy = endPoint->GetKineticEnergy();      
    // analysis->FillH1(ih,energy);
    //
    G4ThreeVector momentum = endPoint->GetMomentum();
    Q        += energy;
    Pbalance += momentum;
    //
    nuclearChannel += partName + " + ";
  }  

  //secondaries
  //
  const std::vector<const G4Track*>* secondary 
                                    = step->GetSecondaryInCurrentStep();  
  for (size_t lp=0; lp<(*secondary).size(); lp++) {
    particle = (*secondary)[lp]->GetDefinition(); 
    G4String name   = particle->GetParticleName();
    G4String type   = particle->GetParticleType();      
    G4double energy = (*secondary)[lp]->GetKineticEnergy();

    if (particle == G4Neutron::Neutron()) {
      if ((partName == "triton") && (targetName == "Deuterium")) {
        analysis->FillH1(16, energy);
      }
        // neutron produced by Triton+Li6 reaction
      if ((partName == "triton") && (targetName == "Li6")) {
        analysis->FillH1(17, energy);
      }
      if ((partName == "triton") && (targetName == "Li7")) {
        analysis->FillH1(17, energy);
      }
    }

   // run->ParticleCount(name,energy);
    //energy spectrum
    ih = 0; 
         if (particle == G4Gamma::Gamma())       ih = 2;
    else if (particle == G4Electron::Electron()) ih = 3;
    else if (particle == G4Neutron::Neutron())   ih = 4;        
    else if (particle == G4Proton::Proton())     ih = 5;
    else if (particle == G4Deuteron::Deuteron()) ih = 6;
    else if (particle == G4Alpha::Alpha())       ih = 7;       
    else if (type == "nucleus")                  ih = 8;
    else if (type == "meson")                    ih = 9;
    else if (type == "baryon")                   ih = 10;        
   // if (ih > 0) analysis->FillH1(ih,energy);
    //atomic mass
    if (type == "nucleus") {
      G4int A = particle->GetAtomicMass();
     // analysis->FillH1(13, A);
    }
    //energy-momentum balance
    G4ThreeVector momentum = (*secondary)[lp]->GetMomentum();
    Q        += energy;
    Pbalance += momentum;
    //count e- from internal conversion together with gamma
    if (particle == G4Electron::Electron()) particle = G4Gamma::Gamma();
    //particle flag
    fParticleFlag[particle]++;
  }
  
  //energy-momentum balance
  //G4double Pbal = Pbalance.mag();
//   run->Balance(Pbal);
//   ih = 11;
//   analysis->FillH1(ih,Q);
//   ih = 12;
//   analysis->FillH1(ih,Pbal);  

  
  // nuclear channel
  const G4int kMax = 16;  
  const G4String conver[] = {"0","","2 ","3 ","4 ","5 ","6 ","7 ","8 ","9 ",
                             "10 ","11 ","12 ","13 ","14 ","15 ","16 "};
  std::map<G4ParticleDefinition*,G4int>::iterator ip;               
  for (ip = fParticleFlag.begin(); ip != fParticleFlag.end(); ip++) {
    particle = ip->first;
    G4String name = particle->GetParticleName();      
    G4int nb = ip->second;
    if (nb > kMax) nb = kMax;   
    G4String Nb = conver[nb];    
    if (particle == G4Gamma::Gamma()) {
     run->CountGamma(nb);
     Nb = "N ";
     name = "gamma or e-";
    } 
    if (ip != fParticleFlag.begin()) nuclearChannel += " + ";
    nuclearChannel += Nb + name;
  }
 



  ///G4cout << "\n nuclear channel: " << nuclearChannel << G4endl;
  run->CountNuclearChannel(nuclearChannel, Q);
    
  fParticleFlag.clear();

 if (edep <= 0.) return;
 
 G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();     
 
 //longitudinal profile of deposited energy
 //randomize point of energy deposition
 //
 //G4StepPoint* prePoint  = step->GetPreStepPoint();
 G4StepPoint* postPoint = step->GetPostStepPoint(); 
 G4ThreeVector P1 = prePoint ->GetPosition();
 G4ThreeVector P2 = postPoint->GetPosition();
 G4ThreeVector point = P1 + G4UniformRand()*(P2 - P1);
 if (step->GetTrack()->GetDefinition()->GetPDGCharge() == 0.) point = P2;
 G4double x = point.x();
 G4double xshifted = x + 0.5*fDetector->GetAbsorSizeX();  
 analysisManager->FillH1(10, xshifted, edep);

 //total energy deposit in absorbers
 //
 G4int iabs = prePoint->GetTouchableHandle()->GetCopyNumber(0); 
 if (iabs > 0) fEventAction->AddEdep(iabs, edep);


}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

