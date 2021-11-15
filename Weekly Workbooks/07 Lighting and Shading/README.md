## Lighting and Shading
### <a href='https://web.microsoftstream.com/group/1dcf4e99-7117-4f18-9936-c5e6712daf00?view=videos' target='_blank'> Weekly Briefing ![](../../resources/icons/briefing.png) </a>
### Task 1: Introduction
 <a href='01%20Introduction/slides/segment-1.pdf' target='_blank'> ![](../../resources/icons/slides.png) </a> <a href='01%20Introduction/audio/segment-1.mp4' target='_blank'> ![](../../resources/icons/audio.png) </a> <a href='01%20Introduction/audio/segment-2.mp4' target='_blank'> ![](../../resources/icons/audio.png) </a> <a href='01%20Introduction/animation/segment-1.mp4' target='_blank'> ![](../../resources/icons/animation.png) </a>

In this workbook we explore some of the more advanced aspects of model rendering. In particular, we will focus on the use of various lighting and shading effects to make our renders look more realistic and aesthetically appealing.

In the slides and audio narration above, we introduce three distinct types of light as a convenient _approximation_ to real-world lighting:

- Diffuse lighting ("general" surface lighting)
- Specular highlighting (to give objects the appearance of a shiny/reflective surface)
- Ambient lighting (as an approximation to "global illumination")

To aid your understanding of this topic, the animation linked to above provides an illustration of the appearance and effect of each type of light. The following sections explore the implementation of these different types of light in more detail. It is important however to you have an understanding of the overall concepts before moving on to the practical tasks in this workbook.  


# 
### Task 2: Proximity Lighting
 <a href='02%20Proximity%20Lighting/slides/segment-1.pdf' target='_blank'> ![](../../resources/icons/slides.png) </a> <a href='02%20Proximity%20Lighting/audio/segment-1.mp4' target='_blank'> ![](../../resources/icons/audio.png) </a> <a href='02%20Proximity%20Lighting/animation/segment-1.mp4' target='_blank'> ![](../../resources/icons/animation.png) </a>

Proximity lighting takes into account the distance of a surface from a light source. Intuitively, the closer a point on a surface is to a light, the brighter it will appear. Take a look at the slides, audio commentary and animation above to gain an understanding of how light dissipates and an appreciation of how we might go about calculating this effect.

Once you have an understanding of the concepts in theory, add code to your renderer that will adjust the brightness of each pixel to take into account proximity of the surface to the light source. Your calculated brightness values should be within the range 0.0 (total darkness) to 1.0 (fully illuminated). These values can then be used as a multiplier to adjust the brightness of a particular pixel. Simply multiply each of the three colour channels (RGB) by the brightness float, before you pack them into the final 32 bit pixel colour.

Your algorithm should be _based_ on the 1/4&pi;r<sup>2</sup> approach outlined in the slides - however, feel free to experiment with multipliers in order to produce an effect that you think looks appropriate (this is were aesthetics rule over science !)  


**Hints & Tips:**  
To find the distance between the light and a point on a triangle, you might like to make use of the GLM `length` function - this will give your the magnitude of a `vec3` variable. Don't normalise your `vec3` otherwise you'll always get a length of 1 !

To help you implement and debug your code, you may wish to add some additional key event handlers that allow you to shift the position of your light source around the model.  


# 
### Task 3: Angle of Incidence Lighting
 <a href='03%20Angle%20of%20Incidence%20Lighting/slides/segment-1.pdf' target='_blank'> ![](../../resources/icons/slides.png) </a> <a href='03%20Angle%20of%20Incidence%20Lighting/audio/segment-1.mp4' target='_blank'> ![](../../resources/icons/audio.png) </a> <a href='03%20Angle%20of%20Incidence%20Lighting/animation/segment-1.mp4' target='_blank'> ![](../../resources/icons/animation.png) </a> <a href='03%20Angle%20of%20Incidence%20Lighting/animation/segment-2.mp4' target='_blank'> ![](../../resources/icons/animation.png) </a>

Your next task is to implement "Angle-of-Incidence" lighting. This lights a surface by considering the orientation of that surfaces relative to the position of a light source. View the two animations linked to above for a high-level explanation of "Angle-of-Incidence" lighting.

In order to implement this kind of lighting, we must first calculate a "normal" for each triangle (a vector that is perpendicular to the surface of the triangle). View the slides and audio narration above to find out how to calculate the normal for a particular surface.

Using the knowledge gained from the slides and narration, calculate the normal of each triangle in the Cornell Box model. To help achieve this, you might like to use the GLM `cross` function to calculate the cross product of two edges of a triangle. You can safely calculate these normals when you first load in the OBJ geometry file since they aren't going to change during the render (unless you start changing the orientation of triangles during some kind of animation). It is worth noting that the `ModelTriangle` class has a `normal` attribute that can be used to store the calculated normal for each triangle. 

Once the surface normals have been calculated, you can then use these to calculate the angle of incidence of light falling onto the surfaces. This is done by taking the dot product of the normal and light direction vectors (as shown in the slides and audio narration above). To calculate dot products, you might like to use the `dot` function provided by GLM.

Once calculated, use these angles of incidence to adjust the brightness of each pixel (in a similar way to that which you did with proximity in the previous task). You should end up with a render that looks like the image shown below. If you have implemented event handlers to move the position of your light point, use this feature to test that your lighting looks realistic from a number of different positions.


  


![](03%20Angle%20of%20Incidence%20Lighting/images/diffuse-lighting.png)

**Hints & Tips:**  
The "winding" order used by the model will impact how you should calculate the surface normals. View <a href="resources/winding.mp4" target="_blank">this video</a> for an explanation of the concept of "winding". The best advice is to just to assume a particular order and try it. You will soon know if you were wrong (all your surfaces will be dark) so you can then just switch to using the other winding order.

When considering the angle that the light hits the surface...  
you **MUST** use: _the direction vector of the LIGHT from the POINT ON THE SURFACE_  
and **NOT**: _the direction vector of the POINT ON THE SURFACE from the LIGHT_  
If you use the wrong one, you will end up lighting the back surface of your triangles !
  


# 
### Task 4: Specular Lighting
 <a href='04%20Specular%20Lighting/slides/segment-1.pdf' target='_blank'> ![](../../resources/icons/slides.png) </a> <a href='04%20Specular%20Lighting/slides/segment-2.pdf' target='_blank'> ![](../../resources/icons/slides.png) </a> <a href='04%20Specular%20Lighting/audio/segment-1.mp4' target='_blank'> ![](../../resources/icons/audio.png) </a> <a href='04%20Specular%20Lighting/audio/segment-2.mp4' target='_blank'> ![](../../resources/icons/audio.png) </a>

Your next task is to implement a "Specular" lighting effect (as illustrated in the introductory animation to this workbook). The slides and audio narration in this current section provide a description of specular light and how to calculate the brightness of points on a specularly illuminated surface.

This process involves calculating the outgoing angle of reflection `R` of an incoming beam from the light. This in itself is more complex than you might initially think. For this reason, we have included an additional slide and audio narration in order to help you calculate this particular element. View these now if you haven't done so already.

When you are ready, implement a specular lighting effect in your renderer. As with the previous tasks, you should again make use of the GLM `dot` function, as well as the surfaces normals that you previously calculated. The image below provides some examples of the kinds of specular highlight you might expect to see for different exponent values:  


![](04%20Specular%20Lighting/images/various-exponents.jpg)

**Hints & Tips:**  
You should use a relatively large exponent in the calculation of specular spread (for example 256). This will ensure that you get a tightly focused spot/beam of light, which will be easier to see than a specular highlight with a wider spread. It can be difficult to distinguish between the various different lighting effects if the parameters used are too subtle !

You may need to move the light and/or camera to an appropriate location in the scene in order to be able to see the specular highlight effect. Remember that specular lighting represents the reflection of the light source, so the camera needs to be in a position where the reflection should be visible.

  


# 
### Task 5: Ambient Lighting
 <a href='05%20Ambient%20Lighting/slides/segment-1.pdf' target='_blank'> ![](../../resources/icons/slides.png) </a> <a href='05%20Ambient%20Lighting/audio/segment-1.mp4' target='_blank'> ![](../../resources/icons/audio.png) </a>

In the real world, light bounces off objects (potentially multiple times) in order to illuminate "hard to reach" areas of a model (even areas that are in shadow). View the slides and audio narration for more insight into this notion of "Global Illumination". As you will see, calculating this kind of indirect lighting would be computationally expensive. It is possible however to implement a plausible approximation to such lighting with very little additional effort.

Two approaches to creating such "ambient" lighting are commonly used:
- Minimum threshold: An IF statement (or call to the `max` function) is used to ensure that the brightness of any point never falls below a certain threshold level.
- Universal supplement: The brightness of a pixel is calculated, then a fixed amount of additional light is added (the same amount to each and every pixel).

Each approach has its own subtle advantages and disadvantages, yet both can be used to achieve the desired effect as illustrated in the image below. Compare the shadow regions of this image with those of the previous week's workbook (which included no ambient lighting).  


![](05%20Ambient%20Lighting/images/shadows-with-ambient.png)

# 
### Task 6: Gouraud Shading
 <a href='06%20Gouraud%20Shading/slides/segment-1.pdf' target='_blank'> ![](../../resources/icons/slides.png) </a> <a href='06%20Gouraud%20Shading/audio/segment-1.mp4' target='_blank'> ![](../../resources/icons/audio.png) </a> <a href='06%20Gouraud%20Shading/audio/segment-2.mp4' target='_blank'> ![](../../resources/icons/audio.png) </a> <a href='06%20Gouraud%20Shading/animation/segment-1.mp4' target='_blank'> ![](../../resources/icons/animation.png) </a>

Due to the fact that we represent models as a series of triangles, any artefacts within a model will have a finite resolution. When rendering a model (unless we have a huge number of triangles) the surfaces may well appear "blocky" and "low-res". But what if we could shade triangles more "intelligently" and blend their edges together ? That would "smooth off" the sharp corners and provide an apparently higher resolution final rendering !

_Gouraud shading_ in one such intelligent approach to surface shading. View the slides and audio narration above to gain an understanding of Gouraud shading. Using this knowledge, implement Gouraud shading in your renderer. In order to achieve this, you will need to make use of _Barycentric coordinates_ to interpolate vertex the brightnesses across the face of each triangle. The operation of Barycentric coordinates is illustrated in the animation at the top of this section.

The image below illustrates the smoothing effect of Gouraud shading - it is important to note that both spheres have the _same_ geometry (the same number of triangles). The surfaces in the sphere on the left are shaded with a flat colour, whereas the sphere on the right is rendered using Gouraud shading.  


![](06%20Gouraud%20Shading/images/gouraud.jpg)

**Hints & Tips:**  
The Cornell Box isn't the most useful model for testing Gouraud shading (it's hard to smooth off the edges of a cube !). For this reason, we have provided a simple <a href="resources/sphere.obj" target="_blank">additional model</a> for you to test your code with.

  


# 
### Task 7: Phong Shading
 <a href='07%20Phong%20Shading/slides/segment-1.pdf' target='_blank'> ![](../../resources/icons/slides.png) </a> <a href='07%20Phong%20Shading/audio/segment-1.mp4' target='_blank'> ![](../../resources/icons/audio.png) </a>

Gouraud shading in fine, but it only goes so far in creating smoothly rendered model surfaces. _Phong shading_ is a more sophisticated approach that takes shading one step further (although it can only achieve this at the cost of additional computational overhead !). Take a look at the slides and audio narration above to gain an understanding of Phong Shading. Adapt your renderer to make use of this more sophisticated approach.

Note that the audio narration makes reference to the concept of "bump maps" (which you may wish to explore in the assignment). We mention them here in order to link this concept into the discussion on Phong shading.

The diagram below illustrates the different results of flat, Gouraud and Phong shading. Again, all three spheres have exactly the same geometry.  


![](07%20Phong%20Shading/images/phong.jpg)

# 
End of Workbook