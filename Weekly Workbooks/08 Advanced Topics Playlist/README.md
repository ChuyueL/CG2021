## Advanced Topics Playlist
### <a href='https://web.microsoftstream.com/group/1dcf4e99-7117-4f18-9936-c5e6712daf00?view=videos' target='_blank'> Weekly Briefing ![](../../resources/icons/briefing.png) </a>
### Task 1: Introduction


This workbook does not contain any explicit practical activities for you to complete. Rather, it provides a "playlist" of slides, audio commentary and animations of advanced rendering topics. Some of these techniques you may wish to implement in your rendering engine, others you may wish only to gain an appreciation of.

If you are taking the coursework assessment variant of this unit, implementing a selection of these in your final assignment should result in a higher grade. If you are taking the examination variant of this unit, you will not be assessed on these topics - however you may wish to explore them for your own interest.  


# 
### Task 2: Perspective Correction
 <a href='02%20Perspective%20Correction/slides/segment-1.pdf' target='_blank'> ![](../../resources/icons/slides.png) </a> <a href='02%20Perspective%20Correction/audio/segment-1.mp4' target='_blank'> ![](../../resources/icons/audio.png) </a> <a href='02%20Perspective%20Correction/animation/segment-1.mp4' target='_blank'> ![](../../resources/icons/animation.png) </a>

There is an issue with the basic texture mapping approach that we used in the rasteriser workbook. The problem is that it doesn't take into account the effect of perspective. This is illustrated in the image below - the left hand side shows the a naive (non-corrected) texture and the right hand side shows the perspective corrected version of the same texture. You should be able to see that the left hand image is clearly not right. Review the slides and audio narration to gain an understanding of the nature of the problem and how we might go about solving it.  


![](02%20Perspective%20Correction/images/perspective-correction.jpg)

# 
### Task 3: Soft Shadow
 <a href='03%20Soft%20Shadow/slides/segment-1.pdf' target='_blank'> ![](../../resources/icons/slides.png) </a> <a href='03%20Soft%20Shadow/audio/segment-1.mp4' target='_blank'> ![](../../resources/icons/audio.png) </a> <a href='03%20Soft%20Shadow/audio/segment-2.mp4' target='_blank'> ![](../../resources/icons/audio.png) </a> <a href='03%20Soft%20Shadow/animation/segment-1.mp4' target='_blank'> ![](../../resources/icons/animation.png) </a>

Up until this point we have taken a simplistic approach to shadows. By only considering single point light sources we can only produce naive and simplistic "hard" shadows (as illustrated in the left hand image below). If we want to implement more realistic "soft" shadows (as illustrated in the right hand image below) we are going to need a more sophisticated approach. View the animation linked to above for a deeper understanding of what we are trying to achieve.

The simplest (but by no means most efficient) way to implement soft shadows is to use multi-point light sources. View the slides and audio narration linked to above for an explanation of how to implement this approach. If you decide to implement soft shadows using this technique, be prepared for a significant increase in rendering time !  


![](03%20Soft%20Shadow/images/soft-shadow.jpg)

# 
### Task 4: Reflective Materials
 <a href='04%20Reflective%20Materials/slides/segment-1.pdf' target='_blank'> ![](../../resources/icons/slides.png) </a> <a href='04%20Reflective%20Materials/audio/segment-1.mp4' target='_blank'> ![](../../resources/icons/audio.png) </a> <a href='04%20Reflective%20Materials/audio/segment-2.mp4' target='_blank'> ![](../../resources/icons/audio.png) </a> <a href='04%20Reflective%20Materials/animation/segment-1.mp4' target='_blank'> ![](../../resources/icons/animation.png) </a>

Rather than rendering just opaque, matt surfaces, it would be nice if we could render more complex materials. The slides, audio narration and animation above provide details of how to render triangles as reflective surfaces - in order to represent materials such as mirrors and metals.  


![](04%20Reflective%20Materials/images/reflective.jpg)

# 
### Task 5: Environment Maps
 <a href='05%20Environment%20Maps/slides/segment-1.pdf' target='_blank'> ![](../../resources/icons/slides.png) </a> <a href='05%20Environment%20Maps/audio/segment-1.mp4' target='_blank'> ![](../../resources/icons/audio.png) </a>

The problem with rendering mirrored surfaces is that they reflect everything that surrounds them ! This can make rendering complex scenes particularly costly. We can however use flat images in the form of "Environment Maps" to capture a view of the surrounding area from the perspective of a 3D object. Because these maps are flat images (rather than 3D models in their own right) this can make the computation of reflections much more efficient.
  


![](05%20Environment%20Maps/images/env-map.jpg)

# 
### Task 6: Refractive Materials
 <a href='06%20Refractive%20Materials/slides/segment-1.pdf' target='_blank'> ![](../../resources/icons/slides.png) </a> <a href='06%20Refractive%20Materials/audio/segment-1.mp4' target='_blank'> ![](../../resources/icons/audio.png) </a> <a href='06%20Refractive%20Materials/animation/segment-1.mp4' target='_blank'> ![](../../resources/icons/animation.png) </a> <a href='06%20Refractive%20Materials/animation/segment-2.mp4' target='_blank'> ![](../../resources/icons/animation.png) </a>

Rather than rendering only opaque surfaces it would be nice if we could transparent materials as well. The slides, audio narration and animations above provide details of how to render triangles as refractive surfaces - in order to represent materials such as glass or water.

Correctly implementing transparent materials is a **difficult** task. This is because we must not only deal with _refraction_ of light, but ALSO the _reflection_ of rays. When the angle of incidence is very shallow (as shown on the right hand side in the image below) the refracted ray does not escape the block. In this situation, the surface of the block acts as a mirror - resulting in internal reflection of the ray.

When attempting an implementation of refractive materials , you will find <a href="https://ricktu288.github.io/ray-optics/" target="_blank">this online optics tool</a> useful in testing and validating you results.
&nbsp;
&nbsp;  


![](06%20Refractive%20Materials/images/internal-reflection.jpg)

# 
### Task 7: Bump Maps
 <a href='07%20Bump%20Maps/slides/segment-1.pdf' target='_blank'> ![](../../resources/icons/slides.png) </a> <a href='07%20Bump%20Maps/audio/segment-1.mp4' target='_blank'> ![](../../resources/icons/audio.png) </a> <a href='07%20Bump%20Maps/animation/segment-1.mp4' target='_blank'> ![](../../resources/icons/animation.png) </a>

Finding the closest intersection between a ray and a set of triangles is a costly activity. This is especially true with complex models when each ray being cast must be checked against a potentially very large number of model. 

What if there was a way for our model to consist of just a few simple triangles, but then "fake" complex geometry using "light & shadows". This may sounds like magic, but bump maps allow us to do just that. Take a look at the slides, narration and animation to find out how this can be done.  


![](07%20Bump%20Maps/images/bump-map.jpg)

# 
### Task 8: Photon Maps
 <a href='08%20Photon%20Maps/slides/segment-1.pdf' target='_blank'> ![](../../resources/icons/slides.png) </a> <a href='08%20Photon%20Maps/audio/segment-1.mp4' target='_blank'> ![](../../resources/icons/audio.png) </a> <a href='08%20Photon%20Maps/animation/segment-1.mp4' target='_blank'> ![](../../resources/icons/animation.png) </a>

Previously we used ambient lighting as an approximation to global illumination. However, this is a very big approximation that prevents us from rendering certain types of specialist lighting effects. _Photon maps_ offer a more sophisticated approach that allow us to precompute patterns of light dispersal in order to (relatively) efficiently render more realistic indirect illumination. Take a look at the slides, narration and animation to gain an insight into this technique.  


![](08%20Photon%20Maps/images/photon-map.jpg)

# 
End of Workbook