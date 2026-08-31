---
title: nock
tags: C, XCB, PAM
---

# Nock
> A tiny and super basic X11 screen locker that I wrote out of pure boredom and as a way to distract myself from college academics :p

## Prelude
One day, as any other, I was doom scrolling on my bed and came across this fantastic [video](https://www.youtube.com/watch?v=CagsZdJ3ZhA) by [theurbanpenguin](https://www.youtube.com/@theurbanpenguin). This piqued my interest about `Pluggable Auth Modules` (more on this later) and after a little more digging I found out that I can make **own** pam modules. 

To do this, I needed to implement one of the [core PAM functions](https://www.linuxjournal.com/article/5940). Here I found out about the `pam_sm_authenticate` function. Judging from the name itself you can tell it has *something* to do with auth and... maybe something to do with Linux lockscreens? *cue the title track*

## Tech Stack
- C 
- XCB 
- PAM
- Other POSIX APIs

## Why this Tech Stack?
So **what** exactly do you need to have a functioning lock screen? Well mainly two things - a way for the user to see your lockscreen, and a way for your app to do the actual locking mechanism. 

Let's address them one by one. First the sort of UI part of the app - on Linux users broadly have two choices when making something that interacts closely with the display server. Those choices spring from the display servers itself. Firstly there is `Xorg` (using the X11 protocol) - the (mostly) old display server. Then there is `Wayland` - the modern and sort of the choice for all computers now. 

### Why I chose Xorg as my display server
To be blunt - I chose to make this over Xorg because I was lazy :p
To be not so blunt - The sort of locking mechanism you can build with Xorg is **much** simpler than what you would need to do if you were working with Wayland. Now why is that? Well Xorg is much older than Wayland, and as a result - you dont need to fiddle with the display server all that much which simplifies what you will be needing to do for this particular usecase.

Time to answer this one **question** as broadly as possible : What exactly should a lockscreen do visually? Well... it should lock the screen (*duh!*) and in doing so prevent the user from accessing the computer untill the correct password is entered. This can be done in a very simplified way on the Xorg display server. You just need a way to **re-direct all input** from the user into your lock screen app and **display a gui** of sorts over the already existing applications. More on how I implemented these later.

Now now now.. why not do the same on Wayland? Wellllll - I saw [the bindings](https://pkg.go.dev/github.com/tuxx/wayland-ext-session-lock-go#section-readme) to do any work with the Wayland protocol and got scared XD.

### XCB vs XLib
With our display server chosen, we now need a **UI library** to do our lockscreen bidding. With Xorg we have two C libraries to work with - **XLib and XCB**. What's the difference?

Xlib is an older standard library and XCB is the newer library.

Thats honestly the only reason I chose XCB over Xlib here for this project. XCB is also synchronous while XLib is asynchronous, might be useful when I eventually decide to make a more fleshed out version of this app.


### User Auth in Linux
Okay now we have our UI library, let's now find a way to actually authenticate the user password. Here, we don't really have the burden of choice as there is only one way to authenticate user passwords in Linux - PAM. So we go with that!

## Technical Background - PAM 
Majority of this section would be exploring what PAM is and how it user account authentication works in Linux.

**PAM or the Pluggable Authentication Modules** is basically a set of unix system libraries that handles authentication and access control. All apps that need to validate user account stuff need not write their own auth logic and can just can PAM do it for them.

There are mainly four different **PAM Management Groups**.
1. `auth` : deals with password verification and other ways to authtenticating with your system like fingerprint and face id.
2. `account` : deals with user permissions for files and things like if a certain account is allowed to login or not.
3. `password` : deals with changing passwords and all other things relating to it.
4. `session` : deals with session managemet.

Here, for a lockscreen application we will ofcourse be using the `auth` module. The app will offload the entire "checking password" part to PAM. But how does it actually do that offloading and asking PAM? Thats where **PAM Conversations** come in!

**A PAM Conversation is a routine or a bridge between your regular apps and PAM.** 
The default PAM conversation works over the shell. For example, whenever you run `su`, the application that throws you inside root : `su` calls PAM which runs the default conversation. One of the flags in this default converation hide the password when typing, which is why you can't see your password when this particular app is run.

But we wont be working over the shell, we have our own GUI! I hear ye I hear ye. This is why for this project, we have a **custom** PAM conversation. Nothing complicated tho, just a way to get the entered password, store it in a `pam_response` struct instance so that my password checking function can use it. Here is my simple implementation : 

```
static int my_conv(int num_msg, const struct pam_message **msg, struct pam_response **resp, void *password) {

	struct pam_response *p = malloc(sizeof(struct pam_response));
	p->resp = (char *)malloc(strlen(password)+1);
	strcpy(p->resp, password);
	*resp = p;
		
	return PAM_SUCCESS;

}
```
Here, `struct pam_response` is as imported struct type from the `<security/_pam_types.h>` header. This function above just is used to fill the `conversation struct`. The PAM modules look for any conversation function with the correct function signature (as above), and calls it. 

Now we need to actually authenticate the entered password against the password stored in your computer. For that we need to **make our own pam files!**
We now have to create a seperate `PAM file`, say something called `nock` in `/etc/pam.d` directory. Why? Because we need a seperate set of "auth rules"  that our lock screen will touch. So now we have a nock file, alongside files like `/etc/pam.d/su` and `/etc/pam.d/sshd` and other such apps. This type of structure is by PAM design : each app having its own set of auth rules, as said before.

Our `nock` pam file will contain the following rules : 
```
auth       include      system-local-login
account    include      system-local-login
```
From before, we know we need the `auth` rule for doing the actual authtenticating for the user account, and we need the `account` rule for checking if t hat particular user account is even allowed to login

> One could also add the password rule if they wanted the application to also be able to change userpasswords 

You also might have noticed this `system-local-login` thingy in our pam file. What's that you ask? To answer that we must read each line of the pam file as in almost a semenatic way : "Auth rule *including* **system-local-login**". There is already a pre-exisitng PAM file called the `system-local-login` which will be used along with the auth rule. Our systems can probably have multiple login/lockscreens and having a common set of configurations is quite useful. It is correct to think of this as a sort of **shared authentication policy**.

> There are also the `.so` files which are actual PAM modules, the code which contains the instructions/code that deals with stored passwords, hashing and un-hashing them to use as per need. We dont really need to worry about any of these, so we treat them as abstractions! :D

I have this tiny [**bash script**](https://github.com/NorSomething/nock/blob/main/install.sh) for my app that does the entire writing your PAM file for you! 
