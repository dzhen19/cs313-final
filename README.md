# cs313-final
This project is a sketch to tree program and has two components: a sketching interface, based off the blobilism assignment; and a mesh viewer, based off the mesh-viewer assignment. 

### Premise
3D modeling of realistic trees has been a hot area of research in computer graphics for many years due to both the complexity of tree structures and the ubiquity of trees in virtual environments. Much work has gone into analyzing the anatomical structures of trees in order to perform rule-based tree generation. However, in the early 2000s, image based tree modeling has overtaken rule based modeling in popularity. In image based modeling, an input (usually a 2d image or sketch) is interpolated into a 3d model with depth, thickness, texture, shading, etc. This demo builds off the research of Makoto Okabe, who in 2005 had already designed sophisticated tree sketching and editing software. Due to time constraints, I was only able to implement the bare minimum. Based on a rudimentary 2d sketch, branch depth is interpolated randomly, resulting in a 3d wire sketch of the input image. 

![c29bdc52-dd55-4536-8eb3-b048dc560de4](https://user-images.githubusercontent.com/55254786/234361544-123109c3-dff6-447f-9adc-dc4fecbb771c.jpeg)


### How to build
The user will need to build each of the sub-directories. For both `blobilism/` and `mesh-viewer/`, create a `build/` directory. `cd` inside `build/` and run `cmake ..` and `make`. The executables will be `bloblism/bin/blobilism` and `mesh-viewer/bin/mesh-viewer` respectively. 

### How to run
I didn't have the time to merge these two projects into one piece of software, so the process for getting the demo to work is a little convoluted. 
1. Run the executable for `blobilism`. A sketching program should appear on your screen. Just left click and drag to draw the trunks and branches of your desired tree. After the initial stroke (which defines the trunk), all subsequent strokes (inferred as branches) will automatically attach their base to the closest existing point. Press `c` to clear all strokes and `s` to save. 
2. Once you have saved your sketch, a file `strokes.txt` should appear in your working directory. Move that file into `mesh-viewer/`. 
3. Now, run the `mesh-viewer` executable to see 3d wire sketch. 

[Video demo](https://www.kapwing.com/videos/644817b312f4a60024a6b6e6)

### How it works
There are two main difficulties in image to tree modeling:
1. Estimating a believable structure of a tree from an image
2. Interpolating the tree's depth from a 2d image

In this implementation, the heavylifting for structure estimation is done in `blobilism/`, whereas depth interpolation is dealt with in `mesh-viewer/`. Taking inspiration from Okabe's tree drawing program, I basically re-implemented `bloblism/` to be a brush stroke tracker. Each brush stroke (defined as the mouse movement between mouseDown and mouseUp) is saved in the following format: 

```
BEGIN_STROKE // delimits start of the stroke
PARENT_STROKE_ID 0 // which stroke is this current stroke attached to? 
PARENT_VERTEX_ID 4 // ^ which vertex? 
VERTICES 0.492 0.526 0.492 0.528 // v0_x, v0_y, v1_x, v1_y, etc. x,y in [0,1]
END_STROKE // delimits end of the stroke
```

All strokes are then stored together in the `strokes.txt` file. This file is then parsed by `mesh-viewer` to build an array of strokes. At this point, the strokes can already be rendered in `mesh-viewer`, although they will be a flat projection on the x-y plane. With the exception of the trunk (which is the first stroke in the array), all strokes (henceforth "branches") are now randomly given a z direction towards which they will extend. After each branch is translated in their chosen z direction, we have to reattach the branch to their parent branch in order to preserve the treelike structure. To do so, we look up the root position for each branch based on their parent stroke and parent vertex, and subtract the difference from each of the branch's vertices. 

### Future work
Due to time constraints, I was unable to implement the method Okabe used to choose z directions for the branches (which leverages a greedy algorithm to maximize distance between branches). I was also unable to get to texturing, generating meshes for the trees, or leaves for that matter. I think I will probably expand on this project for my senior poster and will look into porting this implementation into WebGL for increased accessibility. 
