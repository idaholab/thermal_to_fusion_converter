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
/// \file DetectorMessenger.cc
/// \brief Implementation of the DetectorMessenger class
//
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "DetectorMessenger.hh"

#include <sstream>

#include "DetectorConstruction.hh"
#include "G4UIdirectory.hh"
#include "G4UIcommand.hh"
#include "G4UIparameter.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWithoutParameter.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorMessenger::DetectorMessenger(DetectorConstruction * det)
:fDetector(det)
{ 
  fTestemDir = new G4UIdirectory("/neutron_converter/");
  fTestemDir->SetGuidance(" detector control.");
  
  fDetDir = new G4UIdirectory("/neutron_converter/mat/");
  fDetDir->SetGuidance("detector construction commands");
   
  // breeder material name, thickness
  // /neutron_converter/mat/set_breeder_mat LiD 1 mm 
  
  fAbsorCmd = new G4UIcommand("/neutron_converter/mat/set_breeder_mat",this);
  fAbsorCmd->SetGuidance("Set the neutron converter material and its thickness.");
  fAbsorCmd->SetGuidance("  material name");
  fAbsorCmd->SetGuidance("  thickness (with unit) : t>0.");
  //
  G4UIparameter* MatPrm = new G4UIparameter("material",'s',false);
  MatPrm->SetGuidance("material name");
  fAbsorCmd->SetParameter(MatPrm);
  //    
  G4UIparameter* ThickPrm = new G4UIparameter("thickness",'d',false);
  ThickPrm->SetGuidance("thickness of absorber");
  ThickPrm->SetParameterRange("thickness>0.");
  fAbsorCmd->SetParameter(ThickPrm);
  //
  G4UIparameter* unitPrm = new G4UIparameter("unit",'s',false);
  unitPrm->SetGuidance("unit of thickness");
  G4String unitList = G4UIcommand::UnitsList(G4UIcommand::CategoryOf("mm"));
  unitPrm->SetParameterCandidates(unitList);
  fAbsorCmd->SetParameter(unitPrm);
  //
  fAbsorCmd->AvailableForStates(G4State_PreInit,G4State_Idle);
  fAbsorCmd->SetToBeBroadcasted(false);

  // cladding material name, thickness
  // /neutron_converter/mat/set_cladding_mat SS316 1 mm 

  fCladCmd = new G4UIcommand("/neutron_converter/mat/set_clad_mat",this);
  fCladCmd->SetGuidance("Set the cladding material and its thickness.");
  fCladCmd->SetGuidance("  material name");
  fCladCmd->SetGuidance("  thickness (with unit) : t>0.");
  //
  G4UIparameter* MatPrmClad = new G4UIparameter("material",'s',false);
  MatPrmClad->SetGuidance("material name");
  fCladCmd->SetParameter(MatPrmClad);
  //    
  G4UIparameter* ThickPrmClad = new G4UIparameter("thickness",'d',false);
  ThickPrmClad->SetGuidance("thickness of absorber");
  ThickPrmClad->SetParameterRange("thickness>0.");
  fCladCmd->SetParameter(ThickPrmClad);
  //
  G4UIparameter* unitPrmClad = new G4UIparameter("unit",'s',false);
  unitPrmClad->SetGuidance("unit of thickness");
  unitList = G4UIcommand::UnitsList(G4UIcommand::CategoryOf("mm"));
  unitPrm->SetParameterCandidates(unitList);
  fCladCmd->SetParameter(unitPrm);
  //
  fCladCmd->AvailableForStates(G4State_PreInit,G4State_Idle);
  fCladCmd->SetToBeBroadcasted(false);









  
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorMessenger::~DetectorMessenger()
{
  delete fDetDir;
  delete fTestemDir;
  delete fAbsorCmd;
  delete fCladCmd;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorMessenger::SetNewValue(G4UIcommand* command,G4String newValue)
{    

  if (command == fAbsorCmd)
   {
     G4double tick;
     G4String unt, mat;
     std::istringstream is(newValue);
     is >> mat >> tick >> unt;
     G4String material=mat;
     tick *= G4UIcommand::ValueOf(unt);
     fDetector->SetAbsorMaterial (material);
     fDetector->SetAbsorThickness(tick);
   }
  
  if (command == fCladCmd)
   {
     G4double tick;
     G4String unt, mat;
     std::istringstream is(newValue);
     is >> mat >> tick >> unt;
     G4String material=mat;
     tick *= G4UIcommand::ValueOf(unt);
     fDetector->SetCladMaterial (material);
     fDetector->SetCladThickness(tick);
   }

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
