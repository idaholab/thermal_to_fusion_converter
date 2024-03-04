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
/// \file DetectorConstruction.cc
/// \brief Implementation of the DetectorConstruction class
//
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "DetectorConstruction.hh"
#include "DetectorMessenger.hh"

#include "G4NistManager.hh"
#include "G4Material.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4SubtractionSolid.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4PVReplica.hh"
#include "G4UniformMagField.hh"

#include "G4GeometryManager.hh"
#include "G4PhysicalVolumeStore.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4SolidStore.hh"

#include "G4VisAttributes.hh"
#include "G4SDManager.hh"

#include "G4UImanager.hh"
#include "G4UnitsTable.hh"
#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"
#include "G4GlobalMagFieldMessenger.hh"
#include "G4AutoDelete.hh"
#include "G4RunManager.hh"
#include <iomanip>

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorConstruction::DetectorConstruction()
{
  // default value = 1 mm thickness for neutron converter material
  fAbsorThickness = 1*mm; 
  // materials
  DefineMaterials();
  // create commands for interactive definition of the detector
  fDetectorMessenger = new DetectorMessenger(this);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorConstruction::~DetectorConstruction()
{
    delete fDetectorMessenger;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::DefineMaterials()
{

  G4int ncomponents, natoms;
  G4double abundance ;

  G4double temperature = 344.*kelvin ; // ATR temperature 
  G4double pressure = 26.9*bar ; // ATR pressure 

  G4NistManager* nistManager = G4NistManager::Instance();


  //----------
  // isotopes
  //----------
  G4Isotope* deuterium = new G4Isotope("Deuterium", 1, 2, 2.014 * g/mole);
  G4Isotope* helium3 = new G4Isotope("Helium3", 2, 3, 3.016 * g/mole);
  G4Isotope* lithium6 = new G4Isotope("Li6", 3, 6, 6.015*g/mole);
 

  //----------
  // elements
  //----------

  //natural Li
  G4Element* Li = new G4Element("Lithium", "Li", 3, 6.94 * g / mole);
  //pure Li6
  G4Element* Li6 = new G4Element("Lithium6", "Li6", 1);
  Li6->AddIsotope(lithium6, 1.0);
  // deuterium 
  G4Element* D = new G4Element("Deuterium", "D", 1);
  D->AddIsotope(deuterium,1.0);

  
  // oxygen 
  G4Element* O = new G4Element("Oxygen", "O", 8, 15.99*g/mole);
  // Aluminium 
  G4Element* Al = new G4Element("Aluminium", "Al", 13, 26.982*g/mole);
  // Helium 3
  G4Element* He3 = new G4Element("Helium3", "He3", 1);
  He3->AddIsotope(helium3, 1.0);

  // neutron converter materials
  // LiD
  G4Material* Li6D = new G4Material("Li6D", 0.8 * g / cm3, 2, kStateSolid, temperature, pressure);
  Li6D->AddElement(Li6, 1);  
  Li6D->AddElement(D, 1);


  G4Material* LiD = new G4Material("LiD", 0.8 * g / cm3, 2, kStateSolid, temperature, pressure);
  LiD->AddElement(Li, 1);  
  LiD->AddElement(D, 1);


  G4Material* Li6AlD4 = new G4Material("Li6AlD4", 0.97 * g / cm3, 3, kStateSolid, temperature, pressure);
  Li6AlD4->AddElement(Li6, 1);
  Li6AlD4->AddElement(Al, 1); 
  Li6AlD4->AddElement(D, 4);

  G4Material* LiAlD4 = new G4Material("LiAlD4", 0.97 * g / cm3, 3, kStateSolid, temperature, pressure);
  LiAlD4->AddElement(Li, 1);
  LiAlD4->AddElement(Al, 1); 
  LiAlD4->AddElement(D, 4);


  // pure Li, for testing only 
  G4Material* Li_mat = new G4Material("Li_mat", 0.534 * g / cm3, 1, kStateSolid, temperature, pressure);
  Li_mat->AddElement(Li6, 1);  

  G4Material* He3_mat = new G4Material("He3", 5.273e-04, 1, kStateGas, temperature, pressure);
  He3_mat->AddElement(He3,1.0);
  G4Material* D2 = new G4Material("D2", 7.133e-04, 1, kStateGas, temperature, pressure);
  D2->AddElement(D, 2); 

  // He3D2
  // pressure of 5 Bars
  G4Material* He3D2_1 = new G4Material("He3D2_1", 1.241e-03 * g/cm3, 2 );
  He3D2_1->AddMaterial(He3_mat, 0.5);
  He3D2_1->AddMaterial(D2, 0.5);

  G4Material* He3D2_2 = new G4Material("He3D2_2", 9.769e-04 * g/cm3, 2 );
  He3D2_2->AddMaterial(He3_mat, 1./3);
  He3D2_2->AddMaterial(D2, 2./3);

  G4Material* He3D2_3 = new G4Material("He3D2_3", 8.890e-04 * g/cm3, 2 );
  He3D2_3->AddMaterial(He3_mat, 1./4);
  He3D2_3->AddMaterial(D2, 3./4);


  // D2O
  G4double D2O_dens = 1.1 ;
  G4Material* D2O = new G4Material("D2O", D2O_dens * g / cm3, 2);
  D2O->AddElement(D, 2);
  D2O->AddElement(O, 1);
  
  // Li6OD
  G4double LiOD_dens = 1.46 ;
  G4Material* Li6OD = new G4Material("Li6OD", LiOD_dens * g / cm3, 3, kStateSolid, temperature, pressure);
  Li6OD->AddElement(Li6,1);
  Li6OD->AddElement(O,1);
  Li6OD->AddElement(D,1);
 
  // Li6OD+D2O (0.1,0.9)
  G4double LiOD_mass_frac = 0.1 ;
  G4double mixture_dens = LiOD_mass_frac*LiOD_dens + (1.-LiOD_mass_frac)*D2O_dens;
  G4Material* Li6OD_D2O_10 = new G4Material("Li6OD_D2O_10",mixture_dens * g / cm3, 2);
  Li6OD_D2O_10->AddMaterial(Li6OD,LiOD_mass_frac);
  Li6OD_D2O_10->AddMaterial(D2O,1.-LiOD_mass_frac);

  LiOD_mass_frac = 0.2 ;
  mixture_dens = LiOD_mass_frac*LiOD_dens + (1.-LiOD_mass_frac)*D2O_dens;
  G4Material* Li6OD_D2O_20 = new G4Material("Li6OD_D2O_20",mixture_dens * g / cm3, 2);
  Li6OD_D2O_20->AddMaterial(Li6OD,LiOD_mass_frac);
  Li6OD_D2O_20->AddMaterial(D2O,1.-LiOD_mass_frac);

  LiOD_mass_frac = 0.3 ;
  mixture_dens = LiOD_mass_frac*LiOD_dens + (1.-LiOD_mass_frac)*D2O_dens;
  G4Material* Li6OD_D2O_30 = new G4Material("Li6OD_D2O_30",mixture_dens * g / cm3, 2);
  Li6OD_D2O_30->AddMaterial(Li6OD,LiOD_mass_frac);
  Li6OD_D2O_30->AddMaterial(D2O,1.-LiOD_mass_frac);



  G4Material* LiOD = new G4Material("LiOD",1.46 * g / cm3, 3, kStateSolid, temperature, pressure);
  LiOD->AddElement(Li,1);
  LiOD->AddElement(O,1);
  LiOD->AddElement(D,1);
 

  G4Material* LiOD_D2O_10 = new G4Material("LiOD_D2O_10",1.208 * g / cm3, 2);
  LiOD_D2O_10->AddMaterial(LiOD,0.10);
  LiOD_D2O_10->AddMaterial(D2O,0.90);

  G4Material* LiOD_D2O_20 = new G4Material("LiOD_D2O_20",1.208 * g / cm3, 2);
  LiOD_D2O_20->AddMaterial(LiOD,0.20);
  LiOD_D2O_20->AddMaterial(D2O,0.80);

  G4Material* LiOD_D2O_30 = new G4Material("LiOD_D2O_30",1.208 * g / cm3, 2);
  LiOD_D2O_30->AddMaterial(LiOD,0.30);
  LiOD_D2O_30->AddMaterial(D2O,0.70);


 // define stainless steel 
  G4double A;
  G4double Z;
  G4double d;
  A  =  54.94*g/mole;
  G4Element* elMn   =  new G4Element("Manganese","Mn",Z = 25.,A);
  A = 28.09*g/mole;
  G4Element* elSi  = new G4Element("Silicon","Si",Z = 14.,A); 
  A = 52.00*g/mole;
  G4Element* elCr  = new G4Element("Chromium","Cr",Z = 24.,A); 
  A = 58.70*g/mole;
  G4Element* elNi  = new G4Element("Nickel","Ni",Z = 28.,A); 
  A = 55.85*g/mole;
  G4Element* elFe  = new G4Element("Iron","Fe",Z = 26.,A);

  G4Material* steel = new G4Material("Stainless steel", 8.02*g/cm3 ,5, kStateSolid, temperature, pressure);

  steel->AddElement(elMn, 0.02);
  steel->AddElement(elSi, 0.01);
  steel->AddElement(elCr, 0.19);
  steel->AddElement(elNi, 0.10);
  steel->AddElement(elFe, 0.68);


  // Define Zircaloy material


  G4Element* elZr = new G4Element("Zirconium", "Zr", 40, 91.224 * g / mole);
  G4Element* elSn = new G4Element("Tin", "Sn", 50, 118.71 * g / mole);

  G4Material* zircaloy = new G4Material("Zircaloy", 6.52 * g/cm3, ncomponents = 3, kStateSolid, temperature, pressure);
  zircaloy->AddElement(elZr, 0.98);
  zircaloy->AddElement(elSn, 0.01);
  zircaloy->AddElement(elFe, 0.01);


  // example of vacuum
  G4double density     = universe_mean_density;    //from PhysicalConstants.h
  pressure    = 3.e-18*pascal;
  temperature = 2.73*kelvin;
  G4Material* Galactic = new G4Material("Galactic", 1., 1.008*g/mole, density, kStateGas, temperature, pressure);


  // default materials

  fAbsorMaterial = Li6D ;
  fTargetMaterial = steel ;   
  fCladdingMaterial = zircaloy ;                     
  fDefaultMaterial = Galactic;


  G4cout << *(G4Material::GetMaterialTable()) << G4endl;


}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......


G4VPhysicalVolume* DetectorConstruction::Construct()
{

  G4cout << "DetectorConstruction::Construct() is called" << G4endl;

  //
  // Geometry 
  //

  G4double world_hx = 100*mm ;
  G4double world_hy = 100*mm ;
  G4double world_hz = 100*mm ;

  G4Box* solidWorld = new G4Box("World",world_hx,world_hy,world_hz);  

  G4LogicalVolume* logicWorld = new G4LogicalVolume(solidWorld, fDefaultMaterial, "World");   

  G4VPhysicalVolume *physWorld  = 
    new G4PVPlacement(0,                        //no rotation
                      G4ThreeVector(),        //at (0,0,0)
                      logicWorld,               //logical volume
                      "World",                  //name
                       0,                       //mother volume
                       false,                   //no boolean operation
                       0);                      //copy number
                                 
  //
  // Absorber
  //
  fNbOfAbsor = 1 ; 

  // inner cylinder radius - specimen location fixed at 0.5 cm 
  G4double inner_radius = 5.*mm;
  G4double clad_width = 1.*mm ;
  G4double neutron_converter_width = fAbsorThickness ;
  G4double hz = 2.5*cm; // full lenght = 2*hz
  //needed by other parts of the code
  fAbsorSizeX = neutron_converter_width ;


  G4double startAngle = 0.*deg;
  G4double spanningAngle = 360.*deg;

  // inside specimen full cylinder: 0 to 5 mm radius 
  G4Tubs* specimen 
    = new G4Tubs("specimen",
                  0.*mm,
                  inner_radius,
                  hz,
                  startAngle,
                  spanningAngle);

  // inner clad - full cylinder 0 to 6 mm radius 
  G4Tubs* inner_clad_tmp 
    = new G4Tubs("inner_clad_tmp",
                  0.*mm,
                  inner_radius+clad_width,
                  hz+clad_width,
                  startAngle,
                  spanningAngle);

  // neutron_converter - full cylinder 0 to 7 mm radius 
  G4Tubs* neutron_converter_tmp 
    = new G4Tubs("neutron_converter_tmp",
                  0.*mm,
                  inner_radius+clad_width+neutron_converter_width,
                  hz+clad_width+neutron_converter_width,
                  startAngle,
                  spanningAngle);

  // outer clad - full cylinder 0 to 8 mm radius 
  G4Tubs* outer_clad_tmp 
    = new G4Tubs("outer_clad_tmp",
                  0.*mm,
                  inner_radius+2*clad_width+neutron_converter_width,
                  hz+2*clad_width+neutron_converter_width,
                  startAngle,
                  spanningAngle);

  // do the substraction 
   G4VSolid* inner_clad = new G4SubtractionSolid("inner_clad",inner_clad_tmp,specimen) ;
   G4VSolid* neutron_converter_solid  = new G4SubtractionSolid("neutron_converter",neutron_converter_tmp,inner_clad_tmp)  ;
   G4VSolid* outer_clad = new G4SubtractionSolid("outer_clad",outer_clad_tmp,neutron_converter_tmp)  ;



  specimen_logic = new G4LogicalVolume(specimen, fTargetMaterial,  "specimen"); 

  G4LogicalVolume* inner_clad_logic = new G4LogicalVolume(inner_clad, fCladdingMaterial,  "inner_clad");                             
  G4LogicalVolume* neutron_converter_logic = new G4LogicalVolume(neutron_converter_solid, fAbsorMaterial,  "neutron_converter"); 
  G4LogicalVolume* outer_clad_logic = new G4LogicalVolume(outer_clad, fCladdingMaterial,  "outer_clad"); 

  G4VisAttributes* specimen_vis = new G4VisAttributes(G4Colour(0.5, 0.,0.,1.));  // blue
  specimen_logic->SetVisAttributes(specimen_vis);
  G4VisAttributes* neutron_converter_vis = new G4VisAttributes(G4Colour(0.5, 26,249,7));  // green
  neutron_converter_logic->SetVisAttributes(neutron_converter_vis);
  G4VisAttributes* clad_vis = new G4VisAttributes(G4Colour(0.5, 131,128,123));  // grey
  inner_clad_logic->SetVisAttributes(clad_vis);
  outer_clad_logic->SetVisAttributes(clad_vis);

  G4ThreeVector position = G4ThreeVector(0.,0.,0.);

  new G4PVPlacement(0,                //no rotation
                    position,         //position
                    specimen_logic,   //logical volume        
                    "specimen",       //name
                    logicWorld,       //mother
                    false,            //no boolean operator
                    0,                //copy number
                    true);            //check for overlap  
  new G4PVPlacement(0,                
                    position,         
                    inner_clad_logic,   
                    "inner_clad",             
                    logicWorld,       
                    false,           
                    0,
                    true);               


  new G4PVPlacement(0,                 
                    position,         
                    neutron_converter_logic,            
                    "neutron_converter",             
                    logicWorld,        
                    false,             
                    1, // copy number = 1 for getting secondary particles generated in neutron_converter
                    true);              

  new G4PVPlacement(0,                
                    position,         
                    outer_clad_logic,            
                    "outer_clad",             
                    logicWorld,        
                    false,             
                    0,
                    true);              


  //always return the physical World
  //
  return physWorld;
}


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::ConstructSDandField()
{

  MySensitiveDetector *sensDet = new MySensitiveDetector("SensitiveDetector");
  (G4SDManager::GetSDMpointer())->AddNewDetector( sensDet );
  specimen_logic->SetSensitiveDetector(sensDet);

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::SetAbsorMaterial(const G4String& material)
{
  // search the material by its name

  G4cout << "SetAbsorMaterial is called " << G4endl;
  G4cout << "fAbsorber is set to "<< material << G4endl;
  fAbsorMaterial  = G4Material::GetMaterial(material);
  if (!fAbsorMaterial){
    G4cout << "cannot find material " << material << G4endl;
  }
  G4RunManager::GetRunManager()->PhysicsHasBeenModified();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::SetAbsorThickness(G4double val)
{
  // change Absorber thickness
  //

  if (val <= DBL_MIN)
    { G4cout << "\n --->warning from SetfAbsorThickness: thickness "
             << val  << " out of range. Command refused" << G4endl;
      return;
    }
  fAbsorThickness = val;
  G4RunManager::GetRunManager()->ReinitializeGeometry();
}