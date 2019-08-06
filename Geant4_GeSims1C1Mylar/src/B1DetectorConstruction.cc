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
//
/// \file B1DetectorConstruction.cc
/// \brief Implementation of the B1DetectorConstruction class

#include "B1DetectorConstruction.hh"

#include "G4RunManager.hh"
#include "G4NistManager.hh"
#include "G4Box.hh"
#include "G4Cons.hh"
#include "G4Tubs.hh"
#include "G4Orb.hh"
#include "G4Sphere.hh"
#include "G4Trd.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4SystemOfUnits.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B1DetectorConstruction::B1DetectorConstruction()
: G4VUserDetectorConstruction(),
  fScoringVolume(0),
  fScoringVolume1(0),
  fScoringVolume2(0)
{ }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B1DetectorConstruction::~B1DetectorConstruction()
{ }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* B1DetectorConstruction::Construct()
{  
  // Get nist material manager
  G4NistManager* nist = G4NistManager::Instance();
  
  // Envelope parameters
  //
  G4double env_sizeXY = 100.0*mm, env_sizeZ = 100.0*mm;
  G4Material* env_mat = nist->FindOrBuildMaterial("G4_AIR");
   
  // Option to switch on/off checking of volumes overlaps
  //
  G4bool checkOverlaps = true;

  //     
  // World
  //
  G4double world_sizeXY = 1.2*env_sizeXY;
  G4double world_sizeZ  = 1.2*env_sizeZ;
  G4Material* world_mat = nist->FindOrBuildMaterial("G4_AIR");
  
  G4Box* solidWorld =    
    new G4Box("World",                       //its name
       0.5*world_sizeXY, 0.5*world_sizeXY, 0.5*world_sizeZ);     //its size
      
  G4LogicalVolume* logicWorld =                         
    new G4LogicalVolume(solidWorld,          //its solid
                        world_mat,           //its material
                        "World");            //its name
                                   
  G4VPhysicalVolume* physWorld = 
    new G4PVPlacement(0,                     //no rotation
                      G4ThreeVector(),       //at (0,0,0)
                      logicWorld,            //its logical volume
                      "World",               //its name
                      0,                     //its mother  volume
                      false,                 //no boolean operation
                      0,                     //copy number
                      checkOverlaps);        //overlaps checking
                     
  //     
  // Envelope
  //  
  G4Box* solidEnv =    
    new G4Box("Envelope",                    //its name
        0.5*env_sizeXY, 0.5*env_sizeXY, 0.5*env_sizeZ); //its size
      
  G4LogicalVolume* logicEnv =                         
    new G4LogicalVolume(solidEnv,            //its solid
                        env_mat,             //its material
                        "Envelope");         //its name
               
  new G4PVPlacement(0,                       //no rotation
                    G4ThreeVector(),         //at (0,0,0)
                    logicEnv,                //its logical volume
                    "Envelope",              //its name
                    logicWorld,              //its mother  volume
                    false,                   //no boolean operation
                    0,                       //copy number
                    checkOverlaps);          //overlaps checking
 
  // Create disk shape for Canberra Be3820
  G4Material* shape1_mat = nist->FindOrBuildMaterial("G4_Ge");
  G4ThreeVector pos1 = G4ThreeVector(0., 0., -15.6*mm);
  
  G4double innerRadius = 0.*mm;
  G4double outerRadius = 34.779*mm;
  G4double hz = 10.*mm;
  G4double startAngle = 0.*deg;
  G4double spanningAngle = 360.*deg;
  
  G4Tubs* solidShape1 
    = new G4Tubs("Shape1",
                 innerRadius,
                 outerRadius,
                 hz,
                 startAngle,
                 spanningAngle);
  
  G4LogicalVolume* logicShape1 =                         
    new G4LogicalVolume(solidShape1,         //its solid
                        shape1_mat,          //its material
                        "Shape1");           //its name
               
  new G4PVPlacement(0,                       //no rotation
                    pos1,                    //at position
                    logicShape1,             //its logical volume
                    "Shape1",                //its name
                    logicEnv,                //its mother  volume
                    false,                   //no boolean operation
                    0,                       //copy number
                    checkOverlaps);          //overlaps checking                
  
  // Create disk for carbon window
  G4Material* shape2_mat = nist->FindOrBuildMaterial("G4_C");
  G4ThreeVector pos2 = G4ThreeVector(0., 0., -0.3*mm);
  
  innerRadius = 0.*mm;
  outerRadius = 34.779*mm;
  hz = 0.3*mm;
  startAngle = 0.*deg;
  spanningAngle = 360.*deg;
  
  G4Tubs* solidShape2
    = new G4Tubs("Shape2",
                 innerRadius,
                 outerRadius,
                 hz,
                 startAngle,
                 spanningAngle);
  
  G4LogicalVolume* logicShape2 =                         
    new G4LogicalVolume(solidShape2,         //its solid
                        shape2_mat,          //its material
                        "Shape2");           //its name
               
  new G4PVPlacement(0,                       //no rotation
                    pos2,                    //at position
                    logicShape2,             //its logical volume
                    "Shape2",                //its name
                    logicEnv,                //its mother  volume
                    false,                   //no boolean operation
                    0,                       //copy number
                    checkOverlaps);          //overlaps checking
  
  // Create disk for test source
  
  // Add material definition for Mylar
  G4double A,Z,d;  // atomic mass (mass of a mole)
  
  A = 1.01*g/mole;
  G4Element* elH = new G4Element ("Hydrogen","H",Z = 1.,A);
  A = 16.00*g/mole;
  G4Element* elO = new G4Element("Oxygen","O",Z = 8.,A);
  A = 12.011*g/mole;
  G4Element* elC = new G4Element("Carbon","C",Z = 6.,A);
  d = 1.19*g/cm3;
  G4Material* matplexiglass = new G4Material("Mylar",d,3);
  matplexiglass->AddElement(elH,0.041959);
  matplexiglass->AddElement(elC,0.33303);
  matplexiglass->AddElement(elO,0.625011); 
  
  G4Material* shape3_mat = nist->FindOrBuildMaterial("Mylar");
  G4ThreeVector pos3 = G4ThreeVector(0., 0., 0.749*mm);
  
  innerRadius = 0.*mm;
  outerRadius = 11.9*mm;
  hz = 0.1255*mm;
  startAngle = 0.*deg;
  spanningAngle = 360.*deg;
  
  G4Tubs* solidShape3
    = new G4Tubs("Shape3",
                 innerRadius,
                 outerRadius,
                 hz,
                 startAngle,
                 spanningAngle);
  
  G4LogicalVolume* logicShape3 =                         
    new G4LogicalVolume(solidShape3,         //its solid
                        shape3_mat,          //its material
                        "Shape3");           //its name
               
  new G4PVPlacement(0,                       //no rotation
                    pos3,                    //at position
                    logicShape3,             //its logical volume
                    "Shape3",                //its name
                    logicEnv,                //its mother  volume
                    false,                   //no boolean operation
                    0,                       //copy number
                    checkOverlaps);          //overlaps checking
  
  /*     
  // Shape 1
  // density = 1.396 g/cm^3
  G4Material* shape1_mat = nist->FindOrBuildMaterial("G4_lAr");
  G4ThreeVector pos1 = G4ThreeVector(0., 0., -50.0*cm);
        
  // Box Shape for LAr Moderator
  auto solidShape1 =    
    new G4Box("Shape1", 50.0*cm, 50.0*cm, 50.0*cm);
                      
  G4LogicalVolume* logicShape1 =                         
    new G4LogicalVolume(solidShape1,         //its solid
                        shape1_mat,          //its material
                        "Shape1");           //its name
               
  new G4PVPlacement(0,                       //no rotation
                    pos1,                    //at position
                    logicShape1,             //its logical volume
                    "Shape1",                //its name
                    logicEnv,                //its mother  volume
                    false,                   //no boolean operation
                    0,                       //copy number
                    checkOverlaps);          //overlaps checking 
  
 
  //     
  // Shape 2
  // density = 5.323 g/cm^3
  G4Material* shape2_mat = nist->FindOrBuildMaterial("G4_Ge");
  G4ThreeVector pos2 = G4ThreeVector(0., 0., 50.0*cm);

  // Box Shape for Ge Detector     
  auto solidShape2 =    
    new G4Box("Shape2", 50.0*cm, 50.0*cm, 50.0*cm);
                
  G4LogicalVolume* logicShape2 =                         
    new G4LogicalVolume(solidShape2,         //its solid
                        shape2_mat,          //its material
                        "Shape2");           //its name
               
  new G4PVPlacement(0,                       //no rotation
                    pos2,                    //at position
                    logicShape2,             //its logical volume
                    "Shape2",                //its name
                    logicEnv,                //its mother  volume
                    false,                   //no boolean operation
                    0,                       //copy number
                    checkOverlaps);          //overlaps checking
  */     
          
  // Set Shape2 as scoring volume
  //
  fScoringVolume1 = logicShape1;
  fScoringVolume = logicShape2;
  fScoringVolume2 = logicShape3;
  

  //
  //always return the physical World
  //
  return physWorld;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
