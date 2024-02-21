# CSCE 590 Project 1

## Task 1: Install VS or VS Code

I went with installing VS Code, since I can only access my Azure Portal from
my Linux partition; hence making my only choice VS Code.

![VSCode Install](img/Task1_Install_And_Cfg_VSCode.png)

## Task 2: Create Web App From Azure Portal

I named my project, "csce590-solo-proj1," and decided to use this resource for
the ZIP deployment, VSCode deployment, and slot-deployment tasks.
You can visit the website via the link: [CSCE Solo Project 1](https://csce590-solo-proj1.azurewebsites.net)

![Web App Configuration](img/Task2_Create_Web_App.png)
![Web App Page](img/Task2B_Web_App_Resource_Page.png)

## Task 3: Deploy Sample Code from Portal

For this part, I used the Azure CLI tool, and used the ZIP deployment method,
where you compile a standalone binary for the dotnet project, using the command
below:

```shell
$ dotnet publish
```

After compiling, you then change directories into the target binaries' folder,
and zip the contents of that directory. Then in order to deploy it, use the azure
CLI too, "az", with the following parameters:

```shell
$ az webapp deployment source config-zip --src <zip archive name> \
--resource-group <target resources resource group name> \
--name <target resources name>
```

Zipping the binaries:

![Zip Compiled Binaries](img/Zip_Compiled_Binaries.png)

Deploying the zip-archived binaries to the Web App using Azure-CLI:

![Deploy Zip using Azure-CLI](img/Zip_Deployment_Cmd_And_Confirmation_Json.png)

Confirmation that the deployment was successful:

![Deployment Successful](img/Zip_Deployment_Succeeded.png)

## What I Chose for my Web App

But wait, the screenshot above isn't the sample code! It's not! I decided I 
wanted to go beyond the bare minimum by only deploying the sample code, both 
because I wanted to turn something in I was satisfied with and I was paranoid as
to whether or not you all wanted to see our own work or just the sample code 
uploaded for the later tasks.

##### Quick Side Note:

I didn't know whether or not we were supposed to make our own Web App or not,
as the most information we were given was that the MSLearn Modules provide us
sample code to start with. That being said, when I saw that in the PDF for the App
project, it mentioned that we needed to link you all to the source code repo, 
I was paranoid that I was wrong, and we were expected to create our own web app,
so I decided I would rather be late to turn in something I was actually proud of
rather than just turn in the bare minimum with only the sample code and maybe 
some minimal aesthetic changes to satisfy the code change deployment task.
hence why I turned this in so late.

### My App: A Publicly Available Bulletin Board System

In order to get my project done in a feasible amount of time, I decided to do 
a simple Bulletin Board that uses Dotnet's webapp project template as a starting 
place, as well as EF Core and an Azure Cloud SQL Server to store the posts.
I call it: [Burt's Bulletin Board](https://csce590-solo-proj1.azurewebsites.net).

## Task 4 & 5: Update Web App Thru Code Change (Task 5) and Publish The Changes From Visual Studio (Code)

As said before, I was worried about whether or not y'all wanted us to add our own
nuance to the code we uploaded, so I decided to, yet again, further change the 
web app code we deploy for this task, too. As such, for this part, I decided to
fix an issue that appeared when the home page now had multiple posts to show.
The posts were out of order.

![Bug To Fix](img/BugToFix.png)

They were being ordered by ascending order, rather than descending. Quick and 
easy fix to deploy. Simply had to change the code below from:

![Bug To Fix Code](img/BugToFix_Code.png)

To:

![Bug Fixed Code](img/BugFix_Code.png)

Now, the home page should display the most recent posts first. Now that the code 
has been changed, let's use VSCode's Azure extensions to perform our deployment
more easily. We simply have to go to the side panel tab for the Azure extension,
log into our Azure Portal through the extension's redirect to the login portal,
and find our target resource and hit deploy from its dropdown menu:

![Deployment Option from VSCode Extension](img/Deploy_From_VSCode.png)

![Deployment Confirmation](img/Deploy_Confirmation_VSCode.png)

Now that we've published the changes using the Azure App Service extension for 
VSCode, we can see the changes reflected on the web app home page:

![Bug Fixed and Published](img/BugFix_Via_VSCode.png)

And it worked! Now, the homepage sorts by date, in descending order (i.e.: 
newest posts are at the top).



## Tasks 6 & 9: Create and Configure Deployment Slots (Task 6) & Scale Up Our Web App (Task 9)

I configured my deployment slots to use repos I created on Azure DevOps as the
code source.

![DevOps Repo](img/Repo_For_Website_Src_Code.png)

Before I do that, though, let's Scale Up our resource's pricing plan. 

![Upgrade Prompt](img/Require_Upgrade_For_Slots.png)

Originally, I was at the Free (F1) pricing plan, but in order to
have access to Deployment Slots, I needed to scale up my web app's pricing plan
to S1.

![Scale Up To S1](img/Scale_Up_To_Unlock_Dep_Slots.png)

Now that we have scaled up from F1 to S1, we can implement deployment slot.

![Deployment Slot Configuration](img/Staging_Slot_Repo_Cfg.png)

For the changes to implement for our staging branch, let's fix another logic bug
involving how post bodies are displayed. Since the posts are just being displayed as
paragraph bodies in the html, we need to go through the post body and split it
into an array of strings.

```cs
string[] bodyDelimited = post.Body.split("\n");  // Where \n delimits the string 
                                                 // into substrings in the array.
```

Then use a foreach loop, in the cshtml page for displaying posts, that iterates
through Model.BodyDelimited, and adds that line to the post body paragraph,
with an explicit newline tag,

```html
<br>
```

to explicitly get the browser to render a line break on the web page for each
newline escape char in the post body.

```cs
@foreach (string line in Model.BodyDelimited) {
    @line<br>
}
```

As it stands right now, the webpage for displaying posts only adds line breaks
wherever the browser sees fit, and just disregards any newline chars in the post
body. Which renders ASCII art, for example, complete gibberish.

![Ruined ASCII Art](img/Newline_Bug_To_Fix.png)

My perfectly good burger ASCII art rendered to what just looks like your average
brail poem (or I guess, *feels*, like? Get it? Because it's brail?) Anyway, let's
see how our staging slot's web app looks posting ASCII art now that we've added
code to parse out newline chars and pushed it to the staging branch of our Azure
DevOps repo.

![Newline Parser Implemented on Staging Slot](img/Newline_Bug_Fix.png)

Awesome! The changes are live, oh and I forgot to mention, I also changed any 
mention of, "Burts Bulletin Board," on the webapp, to use proper grammar and 
display, correctly, as, "Burt's Bulletin Board," instead; so, we got to knock out
two birds with one stone! Now let's get prepared to swap the production 
and staging branches!

![Perform Swap Webpage](img/Depslots_Swap.png)

Let's do the honors and click, "Swap!"

![Swap In Progress](img/Swap_In_Progress.png)

Now we wait for it to finish:

![Swap Done](img/Swap_Done.png)

Now let's go to our [production slot's web URL](https://csce590-solo-proj1.azurewebsites.net),
and look at the, "Testing Newline Parser," post, and confirm that it displays our
ASCII art correctly. And it does!

![Swap Live](img/Swap_Changes_Live.png)

#### Quick Side Note: Tasks 7, 8, and 9 Will Be Performed On Containerized App

For tasks 7, 8, and 9, I will be performing these tasks on the Web App we create for
the containerized app instance hosting, since, for that Web App, I will actually be
using the Web App resource to host a containerized Web API, which will allow me to more easily
write a traffic simulating-client that can be used to get an idea of how much our 
metrics logs change after the scale out/up. Therefore, before we do these tasks,
I'm going to now talk about the container tasks, and return to Tasks 7, 8, and 9
after we set up our docker image-based containerized web app.

## Task 10: Create Azure Docker Registry

The container registry I created was simply named csce590, and as the name 
suggests, will only contain docker images pertinent to this class (maybe).

![Container Registry Setup](img/Container_Registry_Create.png)

And here it is after setting it up:

![Container Registry Azure Portal Page](img/ContainerRegistry_Page.png)

After that, I had to install Docker on Linux. This, in spite of previous 
fiddling with containers on Linux in the past, was *surprisingly* easy to do on 
Linux this time around! For my Linux Distro, Arch Linux, the package manager,
pacman, is what I used to install Docker.

```bash
$ sudo pacman -S docker
```

After I installed it, I tested it out with the ArchWiki's recommended test
image, a simple container that uses an Arch Linux image, and just runs

```bash
echo hello, world!
```

![Test Docker Locally Command](img/Test_Docker_Install.png)

After that was successful, I then took the Dockerfile from the sample code repo,
used for the Docker Containerization demo, and edited it to build a container 
image of my Web API instead. 

![Dockerfile Download For Templating](img/Dockerfile_Setup.png)

Which, speaking of, I should mention real quick!

##### (Quick Aside) My Container Project: A Web API That Uses Scoped Services To Handle HTTP Requests

Just like the Web App version, this is a liaison for an Azure SQL Database that
is configured to act as a bulletin board. The main differences here are that
this uses the REST API on ASP.NET, so you don't even have to use a browser to
make posts on the bulletin board. You can use CURL or any other custom client, 
so long as it knows how to make HTTP requests and knows the proper format the API
expects requests to conform to.

Here is a brief little overview of the endpoints set up:

![BulletinBoard-Api Endpoints](img/BBApi_Endpts.png)

So back to the containerization process. I took the Dockerfile from the sample 
project, and edited it to use my project, BulletinBoard-Api, instead:

![Revised Dockerfile](img/DockerfileRevisionsForMyProject.png)

Now that we have our custom Dockerfile, let's build the image!

![Build Cmd](img/DockerBuildCmd.png)

And the result of the build attempt:

![Build Outcome](img/DockerBuild_Outcome.png)

Success! Now we can log into the Container Registry directly through the docker CLI tool!

![Docker CR Login](img/CRLogin.png)

And the result of the login attempt:

![Docker Login Successful](img/CRLogin_Success.png)

Now that we are logged in, let's tag our local build image to an image repo
in our container registry:

![Docker Image Tag and Push](img/DockerContainerPushed.png)

Finally, now that our container has been pushed, we can make a new Web App
resource and make it source from the container in our registry!

![Make New Containerized Web App](img/DockerWebAppSettings.png)

![Review and Create Web App](img/DockerWebAppReviewAndCreate.png)

Now let's make sure it's running properly. Let's get the link:

![Web API Link](img/WebAPI.png)

And try out the /post endpoint, which returns all posts on the DB.

![Web API Test](img/WebAPI_Works.png)

It works! Now to test out continuous deployment! First let's turn it on!

![Set Continuous Deployment](img/Set_Continuous_Deployment.png)

Next, let's fix an issue with the endpoints. I originally messed up the 
routing declarations when writing the controllers' code, so they're all 
being routed from the root domain:

![Endpoints Before Fix](img/Endpoints_Before.png)

The issue was that I wrote the routes with a leading '/' which the compiler
interpreted as meaning route from root, instead of, route from controller's
API route.

```cs
/* For example, say we have a create account endpoint
 * in AccountController. I want the route to be:
 * <domain>/Account/Create
 * but because of how I wrote it, it would route to,
 * <domain>/Create>, instead. Hence why the POST 
 * for a new Account is called Create, and the POST
 * for a new Post is called CreateNew. They were originally
 * both named Create, but since I messed up the routing annotation,
 * they were both  being routed from root domain, and, as such,
 * were conflicting with one another.
 * */

// What I had written:
[HttpPost("/Create")]

// What I needed to write instead:
[Route("Create")]
```

Here's what the endpoints look like now that they're fixed.

![Endpoints After](img/Endpoints_After.png)

And, actually, now that I see it again, I gotta rename /Post/CreateNew
back to /Post/Create since, now that they're properly routed, it won't
conflict with /Account/Create.


**Anyway** after I rewrote the endpoint annotations for the controllers, I then 
rebuilt my local docker image so that the new compiled binaries are on it,
and then pushed the latest build of that image to the container registry!

![Push Changes](img/Docker_Rebuild_And_Push.png)

As you may have noticed, I had to push the image twice because I originally forgot
to retag the container registry repo to be tagged to the *latest* build of my local
project image. **Now, lets test to see that the continuous deployment worked!**

![Continuous Deployment Worked](img/ContinuousDeploymentWorked.png)

As can be seen in the URL, our HTTP GET request for a specific post by GUID 
used the route, \<domain name>/Post/\<post GUID>, which is correctly routed.
Before the changes we made, it would have had to be requested via the route,
\<>domain name/\<post GUID>.

## Task 7, 8, & 9: Configuring Metrics, Scaling Out, Scaling Up

Finally, lets try out scaling on the Bulletin Board API. First let's configure metrics:

![Configured Metrics](img/DockerProj_Metrics.png)

#### Then, let's write a traffic client to simulate heavy server load!

Originally, I was going to make a traffic tester that would hit my state-changing endpoints 
(i.e.: the endpoints that add or remove data from the SQL database), 
but then I realized, 1, that would be a lot more involved than I expected, and 2, 
I didn't want to risk overdrawing my Azure Credits (Spoiler alert: I ended up using 
up all my credit anyway, lol). As such, instead, I decided to just write another endpoint
on a specially-made traffic testing controller and service. The service will be 
added during dependency injection as a transient service, so that we can really 
get the most compute demand out of every request our test client makes on that endpoint.

![Testing Service](img/TrafficTestingService.png)

The test service just has one method, SieveOfEratosthenes, which computes all prime numbers
from 2 to \<upperLimit>. Now let's build and push these changes to our container registry.

![Testing Endpoint is Live](img/TestEndptLive.png)

And it's live! Feel free to try it:

[https://csce590-sp1-dockerdep.azurewebsites.net/TrafficTesting/Sieve?limit=**(number >= 2)**](https://csce590-sp1-dockerdep.azurewebsites.net/TrafficTesting/Sieve?limit=5).

Now let's run the test client [(Visit Test Client Source Code)](https://github.com/bsumner2/)! And let's start it up
with 100 client threads as the parameter!

![Hope I Don't Live to Regret This](img/HopeIDontRegretThis.png)

![Needs More Power](img/NeedMorePower.png)
![More!](https://i.makeagif.com/media/11-15-2019/iC1ukZ.gif)
![May Have Gone Overboard](img/MayHaveGoneOverboard.png)

##### After Letting Server Restabilize...

I let the traffic tester run for a while, and then performed the scale out.

![Scale Out](img/ScaleOut_Cfg.png)

Its effects on performance were visible after letting the client run for a while longer. The CPU Time spike on the left was only a temporary spike, accompanied by a spike in response time. The Traffic Testing Client RNG probably tried requesting really high limits when hitting the Sieve endpoint with their requests.

![Scale Out](img/ScaleOut.png)

Finally, let's scale up our app to the Premium P2v2 pricing plan tier. 
This time, you'll see that the graph change was in the opposite direction.
The CPU Time dropped after the scale up, instead of increasing. I think this
was because of the increased computational power supplied to us by the scale,
up allowing our server to handle these requests at a fraction of the time it normally took.

![Scale Up](img/ScaleUp_Cfg.png)

Scale up confirmation:

![Scale Up Confirmation](img/ScaleUp_Confirmed.png)

![Scale Up Finished](img/ScaleUp.png)

As mentioned before, I believe the increased compute power the scale up 
offered allowed the server to keep pace with the test client and fulfill
its Sieve requests at a fraction of the time it took before scaling up.

