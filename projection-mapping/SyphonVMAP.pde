import VMap.*;
import codeanticode.syphon.*;


/***********************************************************
* EXAMPLE PROVIDED WITH SURFACEMAPPER LIBRARY DEVELOPED BY *
* IXAGON AB.                                               *
* This example shows you how to setup the library and      *
* and display single texture to multiple surfaces.         *
* Check the keyPressed method to see how to access         *
* different settings                                       *
***********************************************************/
VMap vmap;

PImage img;
SyphonClient client;
PGraphics glos;

void settings(){
    PJOGL.profile = 1;
    size(1920,1080, P3D);
    //fullScreen(2);
}

QuadSurface surface;

void setup(){
  //size(800,600);
  glos = createGraphics(width, height, P3D);

  client = new SyphonClient(this);
  //Create new instance of SurfaceMapper
 vmap = new VMap(this, width, height);
  //Creates one surface with subdivision 3, at center of screen
 surface = vmap.createQuadSurface(6, width/2, height/2);
}

void draw(){
  background(0);
  glos.beginDraw();
  glos.clear();
  glos.hint(ENABLE_DEPTH_TEST);
  glos.endDraw();
  
  vmap.shake();
  
  if (vmap.getMode() == VMap.MODE_CALIBRATE){
    vmap.render(glos);
  }
  
  if (vmap.getMode() == VMap.MODE_RENDER){
    for(SuperSurface ss : vmap.getSurfaces()){
        // The first time getImage() is called with 
        // a null argument, it will initialize the PImage
        // object with the correct size.
        img = client.getImage(img); // load the pixels array with the updated image info (slow)
        //img = client.getImage(img, false); // does not load the pixels array (faster)    
        ss.render(glos,img);
     
    }
  }
  
   //display the GLOS to screen
  image(glos,0,0,width,height);
}

void keyPressed(){
  //create a new QUAD surface at mouse pos
  if(key == 'a')
   vmap.addQuadSurface("img.jpg",mouseX,mouseY);
  //create new BEZIER surface at mouse pos
  if(key == 'z')
   vmap.addBezierSurface("img.jpg",mouseX,mouseY);
  //switch between calibration and render mode
  if(key == 'c')vmap.toggleCalibration();
  //increase subdivision of surface
  if(key == 'p'){
    for(SuperSurface ss :vmap.getSelectedSurfaces()){
      ss.increaseResolution();
    }
  }
  //decrease subdivision of surface
  if(key == 'o'){
    for(SuperSurface ss :vmap.getSelectedSurfaces()){
      ss.decreaseResolution();
    }
  }
  //save layout to xml
  if(key == 's')vmap.saveXML("bla.xml");
  //load layout from xml
  if(key == 'l')vmap.loadXML("bla.xml");
  //rotate how the texture is mapped in to the QUAD (clockwise)
  if(key == 'j'){
    for(SuperSurface ss :vmap.getSelectedSurfaces()){
      ss.rotateCornerPoints(0);
    }
  }
  //rotate how the texture is mapped in to the QUAD (counter clockwise)
  if(key == 'k'){
    for(SuperSurface ss :vmap.getSelectedSurfaces()){
      ss.rotateCornerPoints(1);
    }
  }
  //increase the horizontal force on a BEZIER surface
  if(key == 't'){
    for(SuperSurface ss :vmap.getSelectedSurfaces()){
      if (ss instanceof BezierSurface)
        ((BezierSurface)ss).increaseHorizontalForce();
    }
  }
  //decrease the horizontal force on a BEZIER surface  
  if(key == 'y'){
    for(SuperSurface ss :vmap.getSelectedSurfaces()){
      if (ss instanceof BezierSurface)
        ((BezierSurface)ss).decreaseHorizontalForce();
    }
  }
  //increase the vertical force on a BEZIER surface  
  if(key == 'g'){
    for(SuperSurface ss :vmap.getSelectedSurfaces()){
      if (ss instanceof BezierSurface)
        ((BezierSurface)ss).increaseVerticalForce();
    }
  }
  //decrease the vertical force on a BEZIER surface  
  if(key == 'h'){
    for(SuperSurface ss :vmap.getSelectedSurfaces()){
      if (ss instanceof BezierSurface)
        ((BezierSurface)ss).decreaseVerticalForce();
    }
  }
}