---
title: nock
tags: C, XCB, PAM
---

# Nock
A tiny and super basic X11 screen locker that I wrote out of pure boredom and as a way to distract myself from college academics :p

## Prelude
One day, as any other, I was doom scrolling on my bed and came across this fantastic [video](https://www.youtube.com/watch?v=CagsZdJ3ZhA) by [theurbanpenguin](https://www.youtube.com/@theurbanpenguin). This piqued my interest about `Pluggable Auth Modules` (more on this later) and after a little more digging I found out that I can make **own** pam modules. 

To do this, I needed to implement one of the [core PAM functions](https://www.linuxjournal.com/article/5940). Here I found out about the `pam_sm_authenticate` function. Judging from the name itself you can tell it has *something* to do with auth and... maybe something to do with Linux lockscreens? *cue the title track*

## Why this Tech Stack?
So **what** exactly do you need to have a functioning lock screen? Well mainly two things - a way for the user to see your lockscreen, and a way for your app to do the actual locking mechanism. 

Let's address them one by one. First the sort of UI part of the app - on Linux users broadly have two choices when making something that interacts closely with the display server. Those choices spring from the choice the display server itself. Firstly there is `Xorg` (using the X11 protocol) - the (mostly) old display server. Then there is `Wayland` - the modern and sort of the choice for all computers now. 

### Why I chose Xorg as my display server
To be blunt - I chose to make this over Xorg because I was lazy :p
To be not so blunt - The sort of locking mechanism you can build with Xorg is **much** simpler than what you would need to do if you were working with Wayland. Now why is that? Well Xorg is much older than Wayland, and as a result - you dont need to fiddle with the display server all that much which simplifies what you will be needing to do for this particular usecase.

Well, we need to answer this one **question** as broadly as possible : What exactly should a lockscreen do visually? Well... it should lock the screen (*duh!*) and in doing so prevent the user from accessing the computer untill the correct password is entered. This can be done in a very simplified way on the Xorg display server. You just need a way to *re-direct all input* from the user into your lock screen app and **display a gui** of sorts over the already existing applications. More on how I implemented these later.

Now now now.. why not do the same on Wayland? Wellllll - I saw [the bindings](https://pkg.go.dev/github.com/tuxx/wayland-ext-session-lock-go#section-readme) to do any work with the Wayland protocol and got scared XD.

