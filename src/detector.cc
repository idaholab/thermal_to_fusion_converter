#include "detector.hh"
#include "G4Neutron.hh"

MySensitiveDetector::MySensitiveDetector(G4String name) : G4VSensitiveDetector(name)
{
}

MySensitiveDetector::~MySensitiveDetector()
{
}

G4bool MySensitiveDetector::ProcessHits(G4Step *aStep, G4TouchableHistory *ROhist)
{
    // G4cout << "MySensitiveDetector::ProcessHits is called " << G4endl ;

    G4AnalysisManager *analysisManager = G4AnalysisManager::Instance();
    G4Track *track = aStep->GetTrack();
    const G4ParticleDefinition *particle = track->GetParticleDefinition();
    G4double energy = track->GetKineticEnergy();
    // all primary + secondary neutrons 
    if (particle == G4Neutron::Definition()) {
        analysisManager->FillH1(19, energy);  
      // 14 MeV D-T neutrons 
      if (track->GetTrackID()>1 && track->GetParentID()>1) {
        analysisManager->FillH1(18, energy);  
      }
        
    }


    return true;
}