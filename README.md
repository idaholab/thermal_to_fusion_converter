

                     FROG: Fusion neutROn Generator.
              Geant4-based code for spectrum neutron converter project.    

0. INSTALLATION OF GEANT4 TOOLKIT

Detailed instructions for installation on local machine available at:
https://geant4-userdoc.web.cern.ch/UsersGuides/AllGuides/html/InstallationGuide/index.html.


For INL HPC, compilation from source on local directory can be achieved via (update local path as necessary):

```
module load cmake zlib
mkdir projects && cd projects 
git clone https://github.com/Geant4/geant4.git
cd ~/projects/geant4
mkdir build
mkdir install 
cd build 
cmake -DCMAKE_INSTALL_PREFIX=~/projects/geant4/install ..
cmake -DGEANT4_INSTALL_DATA=ON .
make -j8 
make install
```

If successful, a directory `~/projects/geant4/install/share/Geant4/data` should contain all the required libraries.
In addition, the code requires the G4TENDL library for charged light ion transport, which can be downloaded at: https://geant4.web.cern.ch/download/11.2.1.html
Place the library in the same directory. 
Next, go to `~/projects/geant4/install/bin` and edit the file `geant4.sh` and uncomment the line and edit the path to the dataset if installed in not standard location. Add the path for the G4TENDL library: `export G4PARTICLEHPDATA=$GEANT4_DATA_DIR/G4TENDL1.4`.


Before compiling Frog, the geant4.sh file needs to be sourced `source ~/projects/geant4/install/bin/geant4.sh`. 
Load the cmake module `module load cmake`. Then proceed by cloning this repository:
```git clone git@github.inl.gov:nicolas-martin/neutron_converter.git
cd neutron_converter 
mkdir build && cd build 
cmake ..
make -j8 
```
You should have an executable called **frog**. 

The code can be run interactively, or using `frog path_to_macrofile`. 

1. MATERIALS AND GEOMETRY DEFINITION

  The geometry consists of a hollow cylinder. 
  Neutron breeder material (default:LiD) is sandwiched in cladding (default: Zr4).
  The center of the hollow cylinder holds a target material (default: SS316).
  A UI command is available to change the breeder material, as well as its thickness.
  /neutron_converter/mat/set_mat LiD 1 mm

 	
 2- PHYSICS LIST
   
  The "Shielding" reference physics list is selected, with additionally G4IonPhysicsPHP for handling 
  light ion transport (requires TENDL library). G4EmStandardPhysics_option4 is selected for the EM model.
  Several hadronic physics options are controlled by environment variables.
  To select them, see frog.cc

 3- AN EVENT : THE PRIMARY GENERATOR
 
  Neutrons are emitted on the outside surface of the neutron converter using spectrum values from ATR.
  General Source Particle (GPS) model is used to set initial position, kinetics and directions of primary neutrons.
  Directions are set using a cosine law to mimic an isotropic flux in space (see GPS documentation).

 A RUN is a set of events.
 
 4- PHYSICS

  The number of secondary neutrons produced in LiD are tallied along with their kinematic energy.
  The number of total (primary+secondary) neutrons 

 5- HISTOGRAMS
         
  The test has several built-in 1D histograms, which are managed by
  G4AnalysisManager and its Messenger. 
  id = 14: Kinetic energy of secondary tritons
  id = 15: Kinetic energy of primary neutrons emitted (verification against MCNP)
  id = 16: Kinetic energy of secondary neutrons emitted in LiD due to D-T reactions 
  id = 17: Kinetic energy of secondary neutrons emitted in LiD due to Li(T,n) reactions 
  id = 18: Kinetic energy of neutrons in target (both primary = from ATR, and secondary = from converter)

  histogram id 14 is optional and can be invoked using 
 6- TRACKING and STEP MAX
 

 7- VISUALIZATION
 
   The Visualization Manager is set in the main().
   The initialisation of the drawing is done via the commands
   /vis/... in the macro vis.mac. To get visualisation:
   > /control/execute vis.mac
 	
   The tracks are drawn at the end of event, and erased at the end of run.   
   gamma green   
   neutron yellow
   negative particles (e-, ...) red
   positive particles (e+, ions, ...) blue
 	
 8- HOW TO START ?
 
   Execute frog in 'batch' mode from macro files :
   frog   neutron_on_LiD_template.mac

