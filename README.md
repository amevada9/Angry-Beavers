# CS3 Final Project - Angry Beavers

By: Arya Mevada, Arielle Tycko, Mohini Misra, Will Dembski

## Section 0: Summary

The game we are going to make is Angry Beavers, a play on the popular mobile game Angry Birds. We decided on this game because we feel it is the best way to implement our physics engine in an interesting and worthwhile way, while creating a game that most people are familiar with and would enjoy playing. 
Our team consists of four people, and we broke the tasks up based on the basic area that the tasks fall under. Arya will be working with updating the SDL_Wrapper file so we can add text, sound, and numbers. Mohini will be focusing on updating the physics engine to implement torque as a rotational force. Arielle will be focused on gameplay and graphics, allowing the game to be more visually appealing and more fun to play. Finally, Will will implement elasticity features in the physics engine and additional gameplay enhancing features.
Game in a Tweet: “Help the Beavers get back to campus by defeating the Coronavirus!”
Section 1: Gameplay
Here are some details about the game:
### How does your game progress?
Launch beavers to hit viruses across the screen.  All of the viruses must be destroyed before you move on to the next level.  You only have a limited number of beavers, so be strategic in how you use your precious beavers.  
### What are the win and loss conditions?
After all the viruses are eliminated, you may progress to the next level.  If you have beaten the final level, you have won the game!  However, if you run out of beavers before all the viruses are gone, you won’t be able to move on and you have lost the game.  
### Are there levels?
Yes, we will make 3 levels.  The first level will be trivial in order to get the player accustomed to launching beavers.  The next two levels will be harder in order to prove the player’s skill in launching Caltech’s mascot.
### Are there points?
Yes.  Since beavers love destroying wood and other materials in order to build dams, points will be awarded for the total destruction the user causes to the scene.  As the beavers are angry that coronavirus has sent them away from campus, more points will be awarded for every virus eradicated.
 
 
In terms of how the game works, these are how the game will work:
### Controls (keyboard? mouse? tongue?)
The user will click the beaver and drag them down in a slingshot.  When the user releases the beaver, the slingshot will take over and launch the furious beaver towards a virus.
### Physics (how does your game incorporate the physics engine?)
The slingshot will act as a spring force - more stretching will launch the beaver further into the sky. Once it is launched, our demo will use velocity vectors and gravity to exhibit projectile motion. Gravity will be applied to the beaver and other objects to keep them grounded.  As beavers and viruses collide, a lot of our work in building collisions will be important in this game. In addition, a torque abstraction will be added to our physics engine to enhance collisions. 
### Game flow (what does the game look like from start to end for the player?)
The user will enter level one and have a text box pop up with information about how to play the game.  When they beat level one, they move on to the next level and the new level will appear.  If they lose, a message will display that they lost and encourage them to try again.
### Graphics (will you draw polygons? use sprites? make your own vector graphics?)
We will draw polygons to be obstacles and structures that the virus lives in.  The slingshot will also be a polygon. The virus and the beaver will be sprites.
 
## Section 2: Feature Set
Below we indicate the features that each group member is working on. Beside each feature, the number in parentheses indicates the priority. All group members have a mix of priorities. 
### Arya (Focus on SDL)  
* (1) Key Handler (using the mouse to drag the beaver back for projectile motion) 
* (2) Build of different levels (Changing the scene when a new level is reached)
* Coming up with structures for the beavers to hit on the new levels) 
* (3) Text (integrating text with SDL, adding text to the demo) Importantly, adding text to prompt the user (start screen, level ups)
* (4) Sound effects (integrating sound effects with SDL, adding them to the demo)

### Mohini (Focus on Forces, especially Torque)* 
* (2) Torque abstraction (implementing rotational dynamics such that collisions with structures move structures about their center of mass to provide a more realistic visual. This will include writing create_torque in forces.c which will use the center of mass of a body as a fulcrum around which the torque is applied. Also includes writing tests.)
* (3) Different types of beavers (adding beavers with special features to vary how collisions happen in the game)
* (4) Collisions visuals (adding explosion visuals/ sounds during collisions)

### Will (Focus on Elasticity Features)
* (1) Slingshot  (Creating it in the demo and having it exhibit elastic behavior as the user’s mouse pulls back and releases. This will require some collaboration with SDL person (Arya).)
* (2) Elasticity abstraction (In order to make the slingshot look more realistic. This will involve adding forces to represent elasticity and will largely involve spring.)
* (3) Score keeping (adding a score feature to demo to track user’s progress; display on screen) 

### Arielle (Focus on Visuals) 
* (1) Sprites & essential scene objects (drawing/building the sprites for the scene, specifically the beavers and virus. Also includes creating a level-one structure for game play.)
* (2) Materials (giving different materials in the structure that the virus is on such that different materials falling on the virus cause different damage. Also includes implementing the loss of health on the virus side.)
* (4) Background scene (adding background to make the scene not just the items involved in game play)
* *Not given a priority 1 feature because we foresee the torque abstraction taking up a lot of time 


## Section 3: Timeline
This section should assign every feature in the previous section to a particular group member and a particular week they will implement it.
### Arya 
* By 5/29: Have SDL text, mouse, and ideally sound is implemented in SDL_wrapper.h
* By 6/2: Have new Key Handler Made 
* By 6/5: Begin making the different levels beyond any used for testing
* By Due Date: Add sound abstractions and sound files for the game. 

### Will
* By 5/29, a working slingshot.  This means that the user will be able to click and drag and shoot any object(will be made into a beaver later).  To accomplish this, the elasticity abstraction must be well done.
* By 6/1, understand and be able to implement text that is meaningful to the user.  This will allow me to get point totals displayed.  I will also have a point basis for calculating points.
* By 6/5, help the team create levels and implement all the features before.

### Mohini
* By 5/29: Write torque abstractions and tests. Be at the point of debugging torque. 
* By 6/1: Debug torque and integrate in demo with collisions. Add different types of beavers. 
* By 6/5: Add collision effects/visuals.
* 


### Arielle: 
* By 5/29: Create artistic design of the different types of beavers and the virus. Also create design of essential scene objects (obstacles, slingshot, etc). Figure out how to implement the designs into the scene.
* By 6/1: Implement health of viruses and impacts of different materials on the health of the virus.
* By 6/5: Debug any of the previous tasks and create the artistic design for the rest of the scene (things that do not affect gameplay).
